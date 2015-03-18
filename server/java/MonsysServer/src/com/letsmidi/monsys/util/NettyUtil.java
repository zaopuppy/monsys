package com.letsmidi.monsys.util;

import com.letsmidi.monsys.log.MyLogger;
import io.netty.bootstrap.Bootstrap;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;

public class NettyUtil {
    public static ChannelFuture startServer(
            int port, NioEventLoopGroup boss, NioEventLoopGroup worker,
            ChannelHandler handler, ChannelHandler child_handler) {

        MyLogger.i("startServer()");

        ServerBootstrap b = new ServerBootstrap();

        b.group(boss, worker)
                .channel(NioServerSocketChannel.class)
                .handler(handler)
                .childHandler(child_handler)
                .option(ChannelOption.SO_BACKLOG, 128)
                .childOption(ChannelOption.SO_KEEPALIVE, true);

        return b.bind(port);
    }

    public static Bootstrap createClientBootstrap(NioEventLoopGroup worker, ChannelInitializer channelInitializer) {
        Bootstrap b = new Bootstrap();

        b.group(worker)
                .channel(NioSocketChannel.class)
                .handler(channelInitializer)
                .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 60 * 1000)
                .option(ChannelOption.SO_KEEPALIVE, true);
        return b;
    }

    public <T> T connect(String host, int port, Class<T> clazz) {
        return null;
    }
}
