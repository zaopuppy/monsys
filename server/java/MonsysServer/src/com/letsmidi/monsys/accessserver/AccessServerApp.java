package com.letsmidi.monsys.accessserver;


import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;

/**
 * Created by z00302892 on 2015/3/27.
 */
public class AccessServerApp {

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) {

        AccessServerApp app = new AccessServerApp();

        app.start();

    }

    private void start() {
        NioEventLoopGroup sharedWorker = new NioEventLoopGroup();

        listenForClients(sharedWorker);

        listenForInnerServers(sharedWorker);
    }

    private void listenForInnerServers(NioEventLoopGroup worker) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        NettyUtil.startServer(1983, boss, worker,
                new LoggingHandler(LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        //
                    }
                });
    }

    private void listenForClients(NioEventLoopGroup worker) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        NettyUtil.startServer(1984, boss, worker,
                new LoggingHandler(LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        //
                    }
                });
    }
}
