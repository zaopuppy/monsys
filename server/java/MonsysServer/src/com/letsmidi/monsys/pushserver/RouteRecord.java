package com.letsmidi.monsys.pushserver;

/**
 * Created by zhaoyi on 15-6-7.
 */
public class RouteRecord {
    private final int innerSeq;
    private final int extSeq;
    private final String peerId;

    public RouteRecord(int innerSeq, int extSeq, String peerId) {
        this.innerSeq = innerSeq;
        this.extSeq = extSeq;
        this.peerId = peerId;
    }

    public int getInnerSeq() {
        return innerSeq;
    }

    public int getExtSeq() {
        return extSeq;
    }

    public String getPeerId() {
        return peerId;
    }
}
