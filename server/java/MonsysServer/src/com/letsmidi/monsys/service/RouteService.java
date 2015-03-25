package com.letsmidi.monsys.service;


import com.letsmidi.monsys.util.BaseClientConnection;
import io.netty.channel.Channel;
import io.netty.channel.nio.NioEventLoopGroup;

public abstract class RouteService<T> extends BaseClientConnection<T> {

    public static class RouteItem {
        private int sequence;
        private Object data;
    }

    public RouteService(NioEventLoopGroup group) {
        super(group);
    }

    @Override
    public void setChannel(Channel channel) {
        //
    }

    @Override
    public Channel popChannel() {
        return null;
    }

    public boolean start() {
        return false;
    }

    public boolean send() {
        return false;
    }
}
