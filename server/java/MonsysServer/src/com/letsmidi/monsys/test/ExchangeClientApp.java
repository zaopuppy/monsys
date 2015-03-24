package com.letsmidi.monsys.test;

import com.letsmidi.monsys.exchange.ExchangeClientConnection;
import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

public class ExchangeClientApp<T> {
    private void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] argv) {
        ExchangeClientApp app = new ExchangeClientApp();
        try {
            app.start();
        } catch (InterruptedException | ExecutionException | IOException e) {
            e.printStackTrace();
        }
    }

    private void start() throws InterruptedException, ExecutionException, IOException {
        NioEventLoopGroup worker = new NioEventLoopGroup();

        System.out.println("done for 1 connections");

        //Channel channel = connection.channel();
        //ChannelFuture last_future = null;
        //BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        //for (;;) {
        //    if (!channel.isActive()) {
        //        log("disconnected");
        //        break;
        //    }
        //    String line = reader.readLine().trim();
        //    System.out.println("input: [" + line + "]");
        //    if (line.equals("exit")) {
        //        System.out.println("exit");
        //        break;
        //    }
        //
        //    if (line.length() <= 0) {
        //        continue;
        //    }
        //    // last_future = ch.writeAndFlush(line + Delimiters.lineDelimiter());
        //    last_future = processInput(channel, line);
        //}

        //channel.close().sync();

        //if (last_future != null) {
        //    last_future.sync();
        //}

        //channel.close().sync();
    }

    private ChannelFuture processInput(Channel channel, String line) {

        return null;
    }
}
