package com.letsmidi.monsys.test;

import com.letsmidi.monsys.log.Logger;
import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.route.Route;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class RouteClientHandler extends SimpleChannelInboundHandler<Route.RouteMsg> {

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        super.channelActive(ctx);
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Route.RouteMsg msg) throws Exception {
        MyLogger.d("received: [" + msg.getType().name() + "]");
        if (msg.getType() == Route.MsgType.CONNECT) {
            MyLogger.e("token: " + msg.getConnect().getToken());
        }
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();

        super.exceptionCaught(ctx, cause);
    }

}
