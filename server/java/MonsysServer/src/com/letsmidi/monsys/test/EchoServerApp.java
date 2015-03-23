package com.letsmidi.monsys.test;

import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;



/**
 * Created by zero on 3/23/15.
 */
public class EchoServerApp {

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] argv) throws InterruptedException {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        NioEventLoopGroup worker = new NioEventLoopGroup();
        ChannelFuture future = NettyUtil.startServer(
            1981, boss, worker, new LoggingHandler(LogLevel.INFO),
            new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel ch) throws Exception {
                    ChannelPipeline pipeline = ch.pipeline();
                    pipeline.addLast("handler", new EchoServerHandler());
                }
            });
        future.channel().closeFuture().sync();
    }

    private static class EchoServerHandler extends ChannelInboundHandlerAdapter {
        @Override
        public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
            // super.channelRead(ctx, msg);
            log("received: " + msg);
            ctx.writeAndFlush(msg);
        }
    }
}
