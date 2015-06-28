package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.sdk.MonsysConnection;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.util.concurrent.Future;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.channels.Channel;
import java.util.ArrayList;
import java.util.concurrent.ExecutionException;

/**
 * Created by zhaoyi on 15-6-7.
 */
public class ApiClientApp {

    private static final String PUSH_CLIENT_ID = "DEVID-Z";

    private static final SequenceGenerator mIdGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) throws InterruptedException, IOException, ExecutionException {
        NioEventLoopGroup shared_worker = new NioEventLoopGroup();
        MonsysConnection conn = new MonsysConnection("127.0.0.1", 1988, shared_worker);

        ChannelFuture f1 = conn.close();
        log(f1.toString());
        f1.sync();

        Future<Integer> f = conn.login("zao1@gmail.com", "password").sync();
        if (f.isSuccess() && f.get() == 0) {
            log("logged in");
        } else {
            log("failed to login");
            return;
        }

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        for (String line = reader.readLine(); line != null; line = reader.readLine()) {
            if (!conn.channel().isActive()) {
                log("connection break");
                break;
            }
            line = line.trim();
            switch (line) {
                case "fgw-list": {
                    Future<ArrayList<MonsysConnection.Fgw>> future = conn.getFgwList();
                    future.sync();
                    log(future.get().toString());
                    break;
                }
                case "dev-list": {
                    Future<ArrayList<MonsysConnection.Dev>> future = conn.getDevList(PUSH_CLIENT_ID);
                    future.sync();
                    log(future.get().toString());
                    break;
                }
                case "dev-info": {
                    ArrayList<Integer> id_list = new ArrayList<>();
                    id_list.add(0);
                    Future<MonsysConnection.DevInfo> future = conn.getDevInfo(PUSH_CLIENT_ID, 0, id_list);
                    log(future.get().toString());
                    break;
                }
                default: {
                    log("unknown command");
                    break;
                }
            }
        }
    }
}
