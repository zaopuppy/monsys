package com.letsmidi.monsys.test;

import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import com.letsmidi.monsys.protocol.push.Push.Login;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;

public class PushClientHandler extends SimpleChannelInboundHandler<PushMsg> {

//  private ChannelHandlerContext mContext = null;

  @Override
  public void channelActive(ChannelHandlerContext ctx) throws Exception {
    // save context, so we can use it in timer-callback
//    mContext = ctx;
//    mContext.executor().schedule(new Runnable() {
//      @Override
//      public void run() {
//        // System.out.println("PushClientHandler timeout: run()");
//
//        mContext.executor().schedule(this, 1, TimeUnit.SECONDS);
//
//        sendTestMsg(mContext.channel());
//      }
//    }, 1, TimeUnit.SECONDS);

    super.channelActive(ctx);

    login(ctx);
  }

  private void login(ChannelHandlerContext ctx) {
    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.LOGIN);

    Login.Builder login = Login.newBuilder();
    login.setDeviceId("DEVID-Z");

    builder.setLogin(login);

    ctx.writeAndFlush(builder.build());
  }

  private void sendTestMsg(Channel channel) {
    PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.LOGIN);

    Login.Builder login_builder = Login.newBuilder();
    login_builder.setDeviceId("test-device-id");

    builder.setLogin(login_builder);

    channel.writeAndFlush(builder.build());
  }

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    System.out.println("received: [" + msg.getType().name() + "]");
  }

  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
    cause.printStackTrace();

    super.exceptionCaught(ctx, cause);
  }

}
