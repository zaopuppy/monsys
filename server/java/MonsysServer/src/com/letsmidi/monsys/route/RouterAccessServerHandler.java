package com.letsmidi.monsys.route;

import com.letsmidi.monsys.ErrCode;
import com.letsmidi.monsys.handler.RelayHandler;
import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.protocol.route.Route.RouteMsg;
import com.letsmidi.monsys.route.session.RouteSession;
import com.letsmidi.monsys.session.SessionManager;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

// TODO: if client disconnected, clean client automatically
// TODO: session timeout
public class RouterAccessServerHandler extends SimpleChannelInboundHandler<Route.RouteMsg> {

  private final SessionManager mSessionManager;

  public RouterAccessServerHandler(SessionManager session_manager) {
    mSessionManager = session_manager;
  }

  private enum State {
    WAIT_FOR_CONNECT,
    WAIT_FOR_PEER,
    CONNECTED,
  }

  private State mState = State.WAIT_FOR_CONNECT;

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, RouteMsg msg) throws Exception {
    switch (mState) {
      case WAIT_FOR_CONNECT:
        onWaitForConnect(ctx, msg);
        break;
      case WAIT_FOR_PEER:
        onWaitForPeer(ctx, msg);
        break;
      case CONNECTED:
        onConnected(ctx, msg);
        break;
      default:
        break;
    }
  }

  private void onWaitForConnect(ChannelHandlerContext ctx, RouteMsg msg) {
    switch (msg.getType()) {
      case CONNECT:
        processConnectRequest(ctx, msg);
        break;
      default:
        MyLogger.e("bad msg type: " + msg.getType());
        ctx.close();
        break;
    }
  }

  private void onWaitForPeer(ChannelHandlerContext ctx, RouteMsg msg) {
    //
  }

  private void onConnected(ChannelHandlerContext ctx, RouteMsg msg) {
    // do nothing
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) throws Exception {
    super.channelInactive(ctx);

    // TODO: update session state about this channel
  }

  private void processConnectRequest(ChannelHandlerContext ctx, RouteMsg msg) {
    if (msg.getType() != Route.MsgType.CONNECT || !msg.hasConnect()) {
      MyLogger.e("bad request");
      return;
    }

    Route.Connect connect = msg.getConnect();

    RouteSession session = (RouteSession) mSessionManager.find(connect.getToken());
    if (session == null) {
      MyLogger.e("bad token");
      return;
    }

    switch (connect.getClientType()) {
      case CLIENT:
        if (session.client_connected.compareAndSet(false, true)) {
          // success
          session.client_channel = ctx.channel();
        } else {
          // failed, client already connected
          MyLogger.e("CLIENT already here");
          ctx.close();
          return;
        }
        break;
      case FGW:
        if (session.fgw_connected.compareAndSet(false, true)) {
          // success
          session.fgw_channel = ctx.channel();
        } else {
          // failed
          MyLogger.e("FGW already here");
          ctx.close();
          return;
        }
        break;
      default:
        MyLogger.e("Bad client type");
        ctx.close();
        return;
    }

    mState = State.WAIT_FOR_PEER;

    // TODO: wait util all clients connected.
    // TODO: if we remove this handler, will it be freed or not? what will happen if msg is receiving in this step?
    if (session.fgw_connected.get() && session.client_connected.get()) {
      Channel client_channel = session.client_channel;
      Channel fgw_channel = session.fgw_channel;

      client_channel.pipeline().remove(this.getClass());
      client_channel.pipeline().addLast(new RelayHandler(fgw_channel));

      fgw_channel.pipeline().remove(this.getClass());
      fgw_channel.pipeline().addLast(new RelayHandler(client_channel));

      // write response
      sendConnectRsp(client_channel, ErrCode.SUCCESS.value());
      sendConnectRsp(fgw_channel, ErrCode.SUCCESS.value());

      // useless code
      mState = State.CONNECTED;

      // session is useless anymore, once established, should not accept new incomes anymore
      session.close();
    }
  }

  private void sendConnectRsp(Channel ch, int code) {
    RouteMsg.Builder builder = MsgUtil.newRouteMsgBuilder(Route.MsgType.CONNECT_RSP);

    Route.ConnectRsp.Builder connect_rsp = Route.ConnectRsp.newBuilder();
    connect_rsp.setCode(code);

    builder.setConnectRsp(connect_rsp);

    ch.writeAndFlush(builder.build());
  }

}
