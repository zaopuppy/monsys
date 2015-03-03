package com.letsmidi.monsys.login;


import java.util.Collection;
import java.util.concurrent.ConcurrentLinkedQueue;

import io.netty.channel.Channel;

public class InMemInfo {
    public static final InMemInfo INSTANCE = new InMemInfo();

    public static class CommServerInfo {
        public Channel channel = null;
        public String ipV4Addr = null;
        public int port = 0;
    }

    private final ConcurrentLinkedQueue<CommServerInfo> mCommServerList = new ConcurrentLinkedQueue<>();

    public boolean addCommServer(CommServerInfo info) {
        if (mCommServerList.size() >= LoginConfig.MAX_SERVER_NUM) {
            return false;
        }

        mCommServerList.add(info);

        return true;
    }

    public void removeCommServer(CommServerInfo info) {
        mCommServerList.remove(info);
    }

    public Collection<CommServerInfo> getCommServerList() {
        return mCommServerList;
    }

    public CommServerInfo chooseCommServer() {
        if (mCommServerList.size() <= 0) {
            return null;
        }

        return mCommServerList.peek();
    }
}

