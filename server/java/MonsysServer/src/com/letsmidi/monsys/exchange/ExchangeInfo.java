package com.letsmidi.monsys.exchange;


import io.netty.channel.Channel;

public class ExchangeInfo {
    private final String id;
    private final String peerId1;
    private final String peerId2;
    private Channel peerChannel1 = null;
    private Channel peerChannel2 = null;

    public ExchangeInfo(String id, String peerId1, String peerId2) {
        this.id = id;
        this.peerId1 = peerId1;
        this.peerId2 = peerId2;
    }

    public String getId() {
        return id;
    }

    public String getPeerId1() {
        return peerId1;
    }

    public String getPeerId2() {
        return peerId2;
    }

    public Channel getPeerChannel1() {
        return peerChannel1;
    }

    public void setPeerChannel1(Channel peerChannel1) {
        this.peerChannel1 = peerChannel1;
    }

    public Channel getPeerChannel2() {
        return peerChannel2;
    }

    public void setPeerChannel2(Channel peerChannel2) {
        this.peerChannel2 = peerChannel2;
    }
}
