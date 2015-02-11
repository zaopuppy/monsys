package com.letsmidi.monsys.test;

import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;


public class ClientHandler extends SimpleChannelInboundHandler<Client.ClientMsg> {

    //private ChannelHandlerContext mContext = null;

    private void log(String msg) {
        System.out.println(msg);
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        // save context, so we can use it in timer-callback
        //mContext = ctx;
        //mContext.executor().schedule(new Runnable() {
        //    @Override
        //    public void run() {
        //        System.out.println("PushClientHandler timeout: run()");
        //        mContext.executor().schedule(this, 1, TimeUnit.SECONDS);
        //        sendTestMsg(mContext.channel());
        //    }
        //}, 1, TimeUnit.SECONDS);
        log("channelActive");

        super.channelActive(ctx);

        login(ctx);
    }

    private void login(ChannelHandlerContext ctx) {
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.LOGIN);

        Client.Login.Builder login_builder = Client.Login.newBuilder();
        login_builder.setClientType(Client.ClientType.CLIENT_TYPE_ANDROID);
        login_builder.setUserName("zao1@gmail.com");
        login_builder.setPassword("zao1zao1no1");

        builder.setLogin(login_builder);
        ctx.writeAndFlush(builder.build());
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Client.ClientMsg msg) throws Exception {
        System.out.println("received: [" + msg.getType().name() + "]");
        if (msg.getType() == Client.MsgType.LOGIN_RSP) {
            System.out.println("login response code: " + msg.getLoginRsp().getCode());
        }
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        log("exceptionCaught");

        cause.printStackTrace();

        super.exceptionCaught(ctx, cause);
    }

}
