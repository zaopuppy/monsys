package com.letsmidi.monsys.util;


import com.letsmidi.monsys.util.ClientConnection;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.net.InetSocketAddress;

public abstract class BaseClientConnection<T> implements ClientConnection<T> {

    private final NioEventLoopGroup mGroup;

    private Channel mChannel = null;

    public BaseClientConnection(NioEventLoopGroup group) {
        mGroup = group;
    }

    @Override
    public ChannelFuture connect(String host, int port) {
        Channel channel = new NioSocketChannel();

        setChannel(channel);
        //ChannelPipeline pipeline = channel.pipeline();
        //initializePipeline(pipeline);

        channel.config().setOption(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5 * 1000);
        channel.config().setOption(ChannelOption.SO_KEEPALIVE, true);

        ChannelFuture register_future = mGroup.register(channel);

        ChannelPromise promise = channel.newPromise();
        register_future.addListener(future -> channel.connect(new InetSocketAddress(host, port), promise));
        //mChannel = channel;

        return promise;
    }

    // protected abstract void initializePipeline(ChannelPipeline pipeline);
    protected void setChannel(Channel channel) {
        if (mChannel != null && mChannel.isOpen()) {
            mChannel.close();
        }

        mChannel = channel;
    }

    public abstract Channel popChannel();

    @Override
    public Channel channel() {
        return mChannel;
    }

    @Override
    public ChannelFuture write(T msg) {
        return mChannel.write(msg);
    }

    @Override
    public Channel flush() {
        return mChannel.flush();
    }

    @Override
    public ChannelFuture writeAndFlash(T msg) {
        return mChannel.writeAndFlush(msg);
    }

    @Override
    public NioEventLoopGroup group() {
        return mGroup;
    }
}
