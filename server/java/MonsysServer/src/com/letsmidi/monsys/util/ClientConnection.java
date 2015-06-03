package com.letsmidi.monsys.util;

import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;

public interface ClientConnection<T> {

    ChannelFuture connect(String host, int port);

    // void setChannel(Channel channel);

    Channel channel();

    // TODO: T --> byte[]
    ChannelFuture write(T msg);

    Channel flush();

    ChannelFuture writeAndFlush(T msg);

    NioEventLoopGroup group();
}

