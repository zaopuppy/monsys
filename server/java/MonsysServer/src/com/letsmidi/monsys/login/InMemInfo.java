package com.letsmidi.monsys.login;


import java.nio.channels.Channel;
import java.util.concurrent.ConcurrentLinkedQueue;

public class InMemInfo {
    public static final ConcurrentLinkedQueue<Channel> commServerList = new ConcurrentLinkedQueue<>();
}

