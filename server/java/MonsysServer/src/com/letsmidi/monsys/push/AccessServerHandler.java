package com.letsmidi.monsys.push;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.protocol.push.Push.ClientLogin;
import com.letsmidi.monsys.protocol.push.Push.ClientLoginRsp;
import com.letsmidi.monsys.protocol.push.Push.Connect;
import com.letsmidi.monsys.protocol.push.Push.ConnectRsp;
import com.letsmidi.monsys.protocol.push.Push.FGatewayInfo;
import com.letsmidi.monsys.protocol.push.Push.GetFgwListRsp;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.protocol.push.Push.UserRegister;
import com.letsmidi.monsys.protocol.push.Push.UserRegisterRsp;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;
import org.hibernate.Session;

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
  }

  private static final int RECONNECT_WAIT_TIME = 5; // in seconds

  private State mState = State.STATE_WAIT_FOR_LOGIN;
  private String mFgwList = null;

  //private final HashedWheelTimer mTimer;
  //private final SessionManager<Integer> mSessionManager;
  //private IdGenerator mIdGenerator = new IdGenerator(1, 0xFFFFFF);

  private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

  public AccessServerHandler(HashedWheelTimer timer) {
    //mTimer = timer;
    //mSessionManager = new SessionManager<>(timer);
  }

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

  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
    cause.printStackTrace();
    mLogger.severe(cause.toString());
    mLogger.severe("msg: " + cause.getMessage());
    super.exceptionCaught(ctx, cause);
  }

  private void onRead_WaitForLogin(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("onRead_WaitForLogin()");
    switch (msg.getType()) {
      case USER_REGISTER:
        processUserRegister(ctx, msg);
        break;
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

  private void processUserRegister(ChannelHandlerContext ctx, PushMsg msg) {
    if (msg.getType() != MsgType.USER_REGISTER || !msg.hasUserRegister()) {
      mLogger.severe("Bad request: " + msg.getType());
      ctx.close();
      return;
    }

    UserRegister user_register = msg.getUserRegister();

    // check arguments
    if (user_register.getAccount().length() < 3 ||
        user_register.getPassword().length() < 3 ||
        user_register.getNickname().length() < 3) {
      mLogger.severe("Bad parameters");
      ctx.close();
      return;
    }

    Session session = HibernateUtil.getSessionFactory().openSession();

    try {
      AccountInfo account_info =
          (AccountInfo) session.get(AccountInfo.class, user_register.getAccount());
      if (account_info != null) {
        mLogger.warning("account already exists: " + user_register.getAccount());
        ctx.close();
        return;
      }

      // TODO: mail server is needed for mail validation
      //TempAccountInfo temp_account_info =
      //    (TempAccountInfo) session.load(TempAccountInfo.class, user_register.getAccount());
      //if (temp_account_info != null) {
      //  mLogger.warning("this account is waiting for activate");
      //  ctx.close();
      //  return;
      //}
      //
      //mLogger.info("account doesn't exist");
      //
      //// insert into temp table
      //temp_account_info = new TempAccountInfo();
      //temp_account_info.setAccount(user_register.getAccount());
      //temp_account_info.setPassword(user_register.getPassword());
      //temp_account_info.setNickname(user_register.getNickname());
      //temp_account_info.setSalt("abc");
      //temp_account_info.setExpire(new Date());
      //session.beginTransaction();
      //session.save(temp_account_info);
      //session.getTransaction().commit();
      //
      //// send mail (use message queue)
      //sendEmail();

      account_info = new AccountInfo();
      account_info.setAccount(user_register.getAccount());
      account_info.setPassword(user_register.getPassword());
      account_info.setNickname(user_register.getNickname());
      account_info.setFgwList("");
      account_info.setSalt("abc");
      account_info.setStatus(AccountInfo.Status.NORMAL.value());

      session.beginTransaction();
      session.save(account_info);
      session.getTransaction().commit();

      mLogger.info("new account added");

      sendUserRegisterRsp(ctx, msg, 0);

      // done
    } finally {
      session.close();
    }
  }

  // TODO: use stand-alone thread/process/module instead of sending directly
  //private void sendEmail() {
  //  String validate_url = "";
  //}

  private void processGetFgwList(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processGetFgwList");
    if (!msg.hasGetFgwList()) {
      return;
    }

    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.GET_FGW_LIST_RSP);
    builder.setSequence(builder.getSequence());

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
    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());
    private int mRetryTimes;
    private final PushMsg mMsg;

    public WaitForFgwTask(ChannelHandlerContext ctx, PushMsg msg, String device_id, int retry_times) {
      mContext = ctx;
      mDeviceId = device_id;
      mRetryTimes = retry_times;
      mMsg = msg;
    }

    @Override
    public void run() {
      if (mRetryTimes <= 0) {
        mLogger.warning("Timeout");
        mContext.close();
        return;
      }
      --mRetryTimes;

      PushChannelManager.FgwInfo fgw_again = PushChannelManager.INSTANCE.find(mDeviceId);
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

      setupRelayChannels(mContext, mMsg, fgw_again);
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

    PushChannelManager.FgwInfo fgw = PushChannelManager.INSTANCE.find(device_id);
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
      new WaitForFgwTask(ctx, msg, device_id, 2).run();
      return;
    }

    setupRelayChannels(ctx, msg, fgw);
  }

  private void setupRelayChannels(final ChannelHandlerContext ctx, PushMsg msg, PushChannelManager.FgwInfo fgw) {
    fgw.channel.pipeline().remove(PushServerHandler.class);
    fgw.channel.pipeline().addLast(new FgwRelayHandler(ctx.channel()));

    // client
    ctx.channel().pipeline().remove(AccessServerHandler.class);
    ClientRelayHandler handler = new ClientRelayHandler(fgw.channel);
    handler.setFgwList(this.mFgwList);
    ctx.channel().pipeline().addLast(handler);

    sendConnectRsp(ctx, msg, 0);
  }

  private void processClientLogin(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processClientLogin()");

    if (msg.getType() != MsgType.CLIENT_LOGIN || !msg.hasClientLogin()) {
      mLogger.severe("ClientLogin expected, but received: " + msg.getType());
      ctx.close();
      return;
    }

    ClientLogin login = msg.getClientLogin();

    Session session = HibernateUtil.getSessionFactory().openSession();

    try {
      AccountInfo info = (AccountInfo) session.load(AccountInfo.class, login.getAccount());
      if (info == null) {
        mLogger.severe("bad account");
        ctx.close();
        return;
      }

      if (!info.getPassword().equals(login.getPassword())) {
        mLogger.severe("bad password");
        ctx.close();
        return;
      }

      if (info.getStatus() != AccountInfo.Status.NORMAL.value()) {
        mLogger.severe("account not validated yet");
        ctx.close();
        return;
      }

      String fgw_list = info.getFgwList();

      this.mFgwList = fgw_list;

      sendClientLoginRsp(ctx, msg, fgw_list);

      setState(State.STATE_LOGGED_IN);

    } finally {
      session.close();
    }

  }

  private void sendClientLoginRsp(ChannelHandlerContext ctx, PushMsg msg, String fgw_list) {
    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.CLIENT_LOGIN_RSP);
    builder.setSequence(msg.getSequence());

    ClientLoginRsp.Builder rsp = ClientLoginRsp.newBuilder();
    rsp.setCode(0);

    builder.setClientLoginRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void sendConnectRsp(ChannelHandlerContext ctx, PushMsg msg, int code) {
    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.CONNECT_RSP);
    builder.setSequence(msg.getSequence());

    ConnectRsp.Builder rsp = ConnectRsp.newBuilder();
    rsp.setCode(code);

    builder.setConnectRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void sendUserRegisterRsp(ChannelHandlerContext ctx, PushMsg msg, int code) {
    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.USER_REGISTER_RSP);
    builder.setSequence(msg.getSequence());

    UserRegisterRsp.Builder rsp = UserRegisterRsp.newBuilder();
    rsp.setCode(code);

    builder.setUserRegisterRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private State getState() {
    return mState;
  }

  private void setState(State mState) {
    this.mState = mState;
  }

}
