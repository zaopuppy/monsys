package com.letsmidi.monsys.sdk;


import java.util.ArrayList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicBoolean;

import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;

public class Monsys {

    private final static AtomicBoolean initialized = new AtomicBoolean(false);

    private static NioEventLoopGroup globalWorker = null;

    public static boolean init(int numOfWorkerThread) {
        if (!initialized.compareAndSet(false, true)) {
            return false;
        }

        globalWorker = new NioEventLoopGroup(numOfWorkerThread);

        return true;
    }

    public static MonsysClient createClient(NioEventLoopGroup worker, MonsysCallback callback) {
        if (!initialized.get()) {
            return null;
        }
        return new MonsysClient(worker, callback);
    }

    public static MonsysClient createClient(MonsysCallback callback) {
        return createClient(globalWorker, callback);
    }

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] argv) throws ExecutionException, InterruptedException {
        if (!Monsys.init(1)) {
            log("failed to initialize monsys");
            return;
        }

        MonsysClient client = Monsys.createClient(null);
        if (client == null) {
            log("failed to create monsys client");
            return;
        }

        //ChannelFuture future = client.connect();
        //future.get();
        //if (!future.isSuccess()) {
        //    log("failed to connect server");
        //    return;
        //}
        //
        //Future<ArrayList<String>> fgw_list = client.queryFgwList(null);
    }
}
