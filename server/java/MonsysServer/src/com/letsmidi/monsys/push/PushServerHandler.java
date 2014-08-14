package com.letsmidi.monsys.push;

import com.letsmidi.monsys.protocol.push.Push.Login;
import com.letsmidi.monsys.protocol.push.Push.LoginRsp;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import java.util.logging.Logger;

public class PushServerHandler extends SimpleChannelInboundHandler<PushMsg> {
  private final Logger mLogger = Logger.getLogger(PushServer.LOGGER_NAME);

  private enum STATE {
    WAITING_FOR_REG,
    REGISTERED,
  }

  private STATE mState = STATE.WAITING_FOR_REG;

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    // System.out.println("received: [" + msg.getType().name() + "]");
    mLogger.info("current state: " + mState.name());
    switch (mState) {
      case WAITING_FOR_REG:
      {
        // mState = STATE.REGISTERED;
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
    if (!msg.getType().equals(MsgType.LOGIN) || !msg.hasLogin()) {
      mLogger.severe("Loggin first please");
      ctx.close();
      return;
    }

    Login login = msg.getLogin();
    ChannelManager.FgwInfo fgw = new ChannelManager.FgwInfo(login.getDeviceId(), ctx.channel());
    if (!ChannelManager.INSTANCE.add(fgw)) {
      ctx.close();
      return;
    }

    mState = STATE.REGISTERED;

    mLogger.info("Logged in successfully: " + login.getDeviceId());

    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.LOGIN_RSP);

    LoginRsp.Builder login_rsp = LoginRsp.newBuilder();
    login_rsp.setCode(0);

    builder.setLoginRsp(login_rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void onRegistered(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    //
  }

  /**
   * WARNING: An exceptionCaught() event was fired, and it reached at the tail
   * of the pipeline. It usually means the last handler in the pipeline did not
   * handle the exception. java.io.IOException:
   * Too many open files in system
   * at sun.nio.ch.ServerSocketChannelImpl.accept0(Native Method)
   * at sun.nio.ch.ServerSocketChannelImpl.accept(ServerSocketChannelImpl.java:150)
   * at io.netty.channel.socket.nio.NioServerSocketChannel.doReadMessages(NioServerSocketChannel.java:135)
   * at io.netty.channel.nio.AbstractNioMessageChannel$NioMessageUnsafe.read(AbstractNioMessageChannel.java:68)
   * at io.netty.channel.nio.NioEventLoop.processSelectedKey(NioEventLoop.java:507)
   * at io.netty.channel.nio.NioEventLoop.processSelectedKeysOptimized(NioEventLoop.java:464)
   * at io.netty.channel.nio.NioEventLoop.processSelectedKeys(NioEventLoop.java:378)
   * at io.netty.channel.nio.NioEventLoop.run(NioEventLoop.java:350)
   * at io.netty.util.concurrent.SingleThreadEventExecutor$2.run(SingleThreadEventExecutor.java:116)
   * at java.lang.Thread.run(Thread.java:695)
   *
   */
  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
    cause.printStackTrace();

    super.exceptionCaught(ctx, cause);
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) throws Exception {
    mLogger.severe("Channel closed");
    super.channelInactive(ctx);
  }

}
