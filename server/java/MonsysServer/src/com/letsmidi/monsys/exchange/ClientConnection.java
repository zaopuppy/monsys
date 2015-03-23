package com.letsmidi.monsys.exchange;

import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;

public interface ClientConnection<T> {

    ChannelFuture connect(String host, int port);

    // void setChannel(Channel channel);

    Channel channel();

    ChannelFuture write(T msg);

    Channel flush();

    ChannelFuture writeAndFlash(T msg);

    NioEventLoopGroup group();
}

