package com.letsmidi.monsys.push;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.push.Push.Login;
import com.letsmidi.monsys.protocol.push.Push.LoginRsp;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;

import java.util.logging.Logger;

public class PushServerHandler extends SimpleChannelInboundHandler<PushMsg> {
  private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

  private enum STATE {
    WAITING_FOR_REG,
    REGISTERED,
  }

  private STATE mState = STATE.WAITING_FOR_REG;

  //private final SessionManager<Integer> mSessionManager;

  public PushServerHandler(HashedWheelTimer timer) {
    //mSessionManager = new SessionManager<>(timer);
  }

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    // System.out.println("received: [" + msg.getType().name() + "]");
    mLogger.info("current state: " + mState.name());
    switch (mState) {
      case WAITING_FOR_REG:
      {
        onWaitingForReg(ctx, msg);
        break;
      }
      case REGISTERED:
      {
        onRegistered(ctx, msg);
        break;
      }
      default:
        break;
    }
  }

  private void onWaitingForReg(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    mLogger.info("onWaitingForReg: " + msg.getType());
    if (!msg.getType().equals(MsgType.LOGIN) || !msg.hasLogin()) {
      mLogger.severe("Loggin first please");
      ctx.close();
      return;
    }

    Login login = msg.getLogin();
    PushChannelManager.FgwInfo fgw = new PushChannelManager.FgwInfo(login.getDeviceId(), ctx.channel());
    if (!PushChannelManager.INSTANCE.add(fgw)) {
      ctx.close();
      return;
    }

    mState = STATE.REGISTERED;

    mLogger.info("Logged in successfully: " + login.getDeviceId());

    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.LOGIN_RSP);
    builder.setSequence(msg.getSequence());

    LoginRsp.Builder login_rsp = LoginRsp.newBuilder();
    login_rsp.setCode(0);

    builder.setLoginRsp(login_rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void onRegistered(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    mLogger.info("onRegistered: " + msg.getType());
  }

  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
    cause.printStackTrace();
    mLogger.severe(cause.toString());
    mLogger.severe("msg: " + cause.getMessage());
    super.exceptionCaught(ctx, cause);
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) throws Exception {
    mLogger.severe("Channel closed");
    super.channelInactive(ctx);
  }

}
