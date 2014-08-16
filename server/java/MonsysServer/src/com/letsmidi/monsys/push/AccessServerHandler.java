package com.letsmidi.monsys.push;

import com.letsmidi.monsys.database.Database;
import com.letsmidi.monsys.protocol.push.Push.*;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

/**
 * TODO: common response
 * @author zero
 *
 */
public class AccessServerHandler extends SimpleChannelInboundHandler<PushMsg> {

  private enum State {
    STATE_WAIT_FOR_LOGIN,
    STATE_LOGGED_IN,
  };

  private static final int RECONNECT_WAIT_TIME = 5; // in seconds

  private State mState = State.STATE_WAIT_FOR_LOGIN;
  private String mFgwList = null;
  private final Logger mLogger = Logger.getLogger(PushServer.LOGGER_NAME);

  @Override
  public void channelActive(ChannelHandlerContext ctx) throws Exception {
    super.channelActive(ctx);
    mLogger.info("connected from: " + ctx.channel().remoteAddress());
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) throws Exception {
    super.channelInactive(ctx);
    mLogger.info("disconnected from: " + ctx.channel().remoteAddress());
  }

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    switch (getState()) {
      case STATE_WAIT_FOR_LOGIN:
        onRead_WaitForLogin(ctx, msg);
        break;
      case STATE_LOGGED_IN:
        onRead_LoggedIn(ctx, msg);
        break;
      default:
        mLogger.severe("Unknown state: " + getState());
        ctx.close();
        break;
    }
  }

  private void onRead_WaitForLogin(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("onRead_WaitForLogin()");
    switch (msg.getType()) {
      case CLIENT_LOGIN:
        processClientLogin(ctx, msg);
        break;
      default:
        mLogger.severe("Unknown message: " + msg.getType());
        ctx.close();
        break;
    }
  }

  private void onRead_LoggedIn(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("onRead_LoggedIn()");
    switch (msg.getType()) {
      case CONNECT:
        processConnect(ctx, msg);
        break;
      case GET_FGW_LIST:
        processGetFgwList(ctx, msg);
        break;
      default:
        mLogger.severe("Unknown message: " + msg.getType());
        ctx.close();
        break;
    }
  }

  private void processGetFgwList(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processGetFgwList");
    if (!msg.hasGetFgwList()) {
      return;
    }

    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.GET_FGW_LIST_RSP);

    GetFgwListRsp.Builder rsp = GetFgwListRsp.newBuilder();
    rsp.setCode(0);
    FGatewayInfo.Builder info = FGatewayInfo.newBuilder();
    if (this.mFgwList != null) {
      String[] fgws = this.mFgwList.split("\\|");
      for (String fgw: fgws) {
        info.setId(fgw);
        info.setName(fgw);
        info.setDesc("nothing about this gateway");
        rsp.addFgwInfos(info.build());
      }
    }

    builder.setGetFgwListRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private class WaitForFgwTask implements Runnable {
    private final String mDeviceId;
    private final ChannelHandlerContext mContext;
    private final Logger mLogger = Logger.getLogger(PushServer.LOGGER_NAME);
    private int mRetryTimes;

    public WaitForFgwTask(ChannelHandlerContext ctx, String device_id, int retry_times) {
      mContext = ctx;
      mDeviceId = device_id;
      mRetryTimes = retry_times;
    }

    @Override
    public void run() {
      if (mRetryTimes <= 0) {
        mLogger.warning("Timeout");
        mContext.close();
        return;
      }
      --mRetryTimes;

      ChannelManager.FgwInfo fgw_again = ChannelManager.INSTANCE.find(mDeviceId);
      if (fgw_again == null) {
        mLogger.severe("fgw still not connected");
        // mContext.close();
        mContext.executor().schedule(this, RECONNECT_WAIT_TIME, TimeUnit.SECONDS);
        return;
      }

      if (!fgw_again.connected.compareAndSet(false, true)) {
        mLogger.severe("Still not fast enough, fail");
        mContext.close();
        return;
      }

      setupRelayChannels(mContext, fgw_again);
    }
  }

  private void processConnect(final ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processConnect()");

    if (msg.getType() != MsgType.CONNECT || !msg.hasConnect()) {
      mLogger.severe("connect expected, but received: " + msg.getType());
      ctx.close();
      return;
    }

    Connect connect_req = msg.getConnect();

    final String device_id = connect_req.getDeviceId();

    ChannelManager.FgwInfo fgw = ChannelManager.INSTANCE.find(device_id);
    if (fgw == null) {
      mLogger.severe("gw not connected yet");
      ctx.close();
      return;
    }

    mLogger.info("gw was found");

    if (!fgw.connected.compareAndSet(false, true)) {
      // close and wait
      mLogger.info("close existing client channel then wait");
      fgw.channel.close();
      // ctx.executor().schedule(new WaitForFgwTask(ctx, device_id, 2), RECONNECT_WAIT_TIME, TimeUnit.SECONDS);
      new WaitForFgwTask(ctx, device_id, 2).run();
      return;
    }

    setupRelayChannels(ctx, fgw);
  }

  private void setupRelayChannels(final ChannelHandlerContext ctx, ChannelManager.FgwInfo fgw) {
    fgw.channel.pipeline().remove(PushServerHandler.class);
    fgw.channel.pipeline().addLast(new FgwRelayHandler(ctx.channel()));

    // client
    ctx.channel().pipeline().remove(AccessServerHandler.class);
    ClientRelayHandler handler = new ClientRelayHandler(fgw.channel);
    handler.setFgwList(this.mFgwList);
    ctx.channel().pipeline().addLast(handler);

    sendConnectRsp(ctx, 0);
  }

  private void processClientLogin(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processClientLogin()");

    // --- FOR DEBUGGING ONLY ---
    if (msg.getType() != MsgType.CLIENT_LOGIN || !msg.hasClientLogin()) {
      mLogger.severe("ClientLogin expected, but received: " + msg.getType());
      ctx.close();
      return;
    }

    ClientLogin login = msg.getClientLogin();

    Connection conn = null;
    Statement stmt = null;
    ResultSet result = null;
    try {
      conn = Database.INSTANCE.getConnection();
      stmt = conn.createStatement();
      result = stmt.executeQuery(
          String.format(
              "select fgw_list from account_info where account = '%s' and password = '%s'",
              login.getAccount(), login.getPassword()));
      if (!result.next()) {
        mLogger.severe("Empty result set");
        ctx.close();
        return;
      }

      String fgw_list = result.getString(1);

      this.mFgwList = fgw_list;

      sendClientLoginRsp(ctx, fgw_list);

      setState(State.STATE_LOGGED_IN);

    } catch (SQLException e) {
      e.printStackTrace();
    } finally {
      if (result != null) {
        try { result.close(); } catch (Exception e) { e.printStackTrace(); }
      }
      if (stmt != null) {
        try { stmt.close(); } catch (Exception e) { e.printStackTrace(); }
      }
      if (conn != null) {
        try { conn.close(); } catch (Exception e) { e.printStackTrace(); }
      }
    }
  }

  private void sendClientLoginRsp(ChannelHandlerContext ctx, String fgw_list) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.CLIENT_LOGIN_RSP);

    ClientLoginRsp.Builder rsp = ClientLoginRsp.newBuilder();
    rsp.setCode(0);

    //        if (fgw_list != null) {
    //            for (String fgw: fgw_list.split("\\|")) {
    //                if (fgw.length() <= 0) {
    //                    continue;
    //                }
    //                FGatewayInfo.Builder info = FGatewayInfo.newBuilder();
    //                info.set
    //                rsp.addFgwInfos(value);
    //            }
    //        }

    builder.setClientLoginRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void sendConnectRsp(ChannelHandlerContext ctx, int code) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.CONNECT_RSP);

    ConnectRsp.Builder rsp = ConnectRsp.newBuilder();
    rsp.setCode(code);

    builder.setConnectRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private State getState() {
    return mState;
  }

  private void setState(State mState) {
    this.mState = mState;
  }

}
