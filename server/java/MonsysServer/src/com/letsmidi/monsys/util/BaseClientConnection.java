package com.letsmidi.monsys.util;


import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;

public abstract class BaseClientConnection<T> implements ClientConnection<T> {

    public static interface Callback<T> {
        void onResponse(T msg);
    }

    public static class RouteItem<T> {
        private final int sequence;
        private final Callback<T> callback;

        public RouteItem(int sequence, Callback<T> callback) {
            this.sequence = sequence;
            this.callback = callback;
        }

        public int getSequence() {
            return sequence;
        }

        public Callback<T> getCallback() {
            return callback;
        }
    }

    private final Map<Integer, RouteItem<T>> mRouteMap = new HashMap<>();
    private final NioEventLoopGroup mGroup;
    private Channel mChannel = null;

    public BaseClientConnection(NioEventLoopGroup group) {
        mGroup = group;
    }

    @Override
    public ChannelFuture connect(String host, int port) {
        Channel channel = new NioSocketChannel();

        setChannel(channel);

        channel.config().setOption(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5 * 1000);
        channel.config().setOption(ChannelOption.SO_KEEPALIVE, true);

        ChannelFuture register_future = mGroup.register(channel);

        ChannelPromise promise = channel.newPromise();
        register_future.addListener(future -> channel.connect(new InetSocketAddress(host, port), promise));

        return promise;
    }

    protected void setChannel(Channel channel) {
        if (mChannel != null && mChannel.isOpen()) {
            mChannel.close();
        }

        mChannel = channel;
    }

    /**
     *
     * @return cleaned up channel
     */
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

    protected void write(T msg, Callback<T> callback) {
        if (!saveRoute(msg, callback)) {
            return;
        }
        // TODO
        // channel.write(msg);
    }

    public Map<Integer, RouteItem<T>> getRouteMap() {
        return mRouteMap;
    }

    protected abstract boolean saveRoute(T msg, Callback<T> callback);

    protected abstract RouteItem<T> findRoute(T msg);

    private void onResponse(T msg) {
        RouteItem<T> item = findRoute(msg);
        if (item == null) {
            return;
        }

        if (item.getCallback() != null) {
            item.getCallback().onResponse(msg);
        }
    }
}
