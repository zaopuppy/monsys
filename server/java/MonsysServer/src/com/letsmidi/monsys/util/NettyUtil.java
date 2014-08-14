package com.letsmidi.monsys.util;

import com.letsmidi.monsys.log.MyLogger;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelOption;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;

public class NettyUtil {
    public static ChannelFuture startServer(
        int port, NioEventLoopGroup boss, NioEventLoopGroup worker,
        ChannelHandler handler, ChannelHandler child_handler) {

        MyLogger.i("startPushServer()");

        ServerBootstrap b = new ServerBootstrap();

        b.group(boss, worker)
         .channel(NioServerSocketChannel.class)
         .handler(handler)
         .childHandler(child_handler)
         .option(ChannelOption.SO_BACKLOG, 128)
         .childOption(ChannelOption.SO_KEEPALIVE, true);

        return b.bind(port);
    }
}
