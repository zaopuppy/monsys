package com.letsmidi.monsys.sdk;

import io.netty.channel.nio.NioEventLoopGroup;

/**
 * Created by zero on 6/17/15.
 */
public class MonsysClient {
    // TODO: need to unregister connection
    private static final NioEventLoopGroup sWorker = new NioEventLoopGroup(1);
    private static final MonsysConnection sConnection = new MonsysConnection("letsmidi.wicp.net", 1988, sWorker);

    public static MonsysConnection getConnection() {
        return sConnection;
    }
}

