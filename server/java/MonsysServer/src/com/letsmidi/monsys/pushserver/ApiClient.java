package com.letsmidi.monsys.pushserver;

import io.netty.channel.Channel;

/**
 * Created by zhaoyi on 15-6-7.
 */
public class ApiClient {
    private final String id;
    private final Channel channel;

    public ApiClient(String id, Channel channel) {
        this.id = id;
        this.channel = channel;
    }

    public String getId() {
        return id;
    }

    public Channel getChannel() {
        return channel;
    }
}
