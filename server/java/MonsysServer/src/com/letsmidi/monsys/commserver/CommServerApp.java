package com.letsmidi.monsys.commserver;

import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.log.MyLogFormatter;
import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MonsysException;
import com.letsmidi.monsys.util.NettyUtil;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.util.HashedWheelTimer;


public class CommServerApp {
    private final Logger mLogger = Logger.getLogger(CommConfig.LoggerName);

    public static void main(String[] args) throws IOException, MonsysException {

        initLogger();

        Class[] mapping_classes = new Class[] {
                AccountInfo.class,
        };

        // initialize hibernate
        if (!HibernateUtil.init(mapping_classes)) {
            System.out.println("Failed to initialize hibernate, failed");
            return;
        }


        CommServerApp server = new CommServerApp();
        server.start();
    }

    private static void initLogger() throws IOException {
        Handler log_handler = new FileHandler(CommConfig.LoggerFileName, 1 << 20, 10000, true);

        final Logger logger = Logger.getLogger(CommConfig.LoggerName);
        logger.setLevel(Level.ALL);
        logger.addHandler(log_handler);

        for (Handler h: logger.getHandlers()) {
            System.out.println("handler: " + h.getClass().getCanonicalName());
            h.setFormatter(new MyLogFormatter());
        }

    }

    public void start() {
        mLogger.info("-----------------------------------");
        mLogger.info("comm server start");

        // global timer
        final HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
        timer.start();

        ArrayList<EventLoopGroup> group_list = new ArrayList<>();
        ArrayList<ChannelFuture> future_list = new ArrayList<>();

        // share worker
        NioEventLoopGroup shared_worker = new NioEventLoopGroup();
        group_list.add(shared_worker);

        // connect to login server
        Bootstrap bootstrap = new Bootstrap();
        bootstrap.group(shared_worker)
                .channel(NioSocketChannel.class)
                .handler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(CommServer.CommServerMsg.getDefaultInstance()),
                                new LoginServerHandler());
                    }
                })
                .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 60 * 1000)
                .option(ChannelOption.SO_KEEPALIVE, true);

        ChannelFuture login_future = bootstrap.connect(CommConfig.loginSererIp, CommConfig.LoginServerPort);
        future_list.add(login_future);

        // listen port for client
        // listen clients
        NioEventLoopGroup client_boss = new NioEventLoopGroup(1);
        ChannelFuture client_future = NettyUtil.startServer(
                CommConfig.ClientListenPort, client_boss, shared_worker,
                new LoggingHandler(CommConfig.LoggerName, LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(Client.ClientMsg.getDefaultInstance()),
                                new ClientHandler(timer));
                    }
                }
        );
        group_list.add(client_boss);
        future_list.add(client_future);


        group_list.add(shared_worker);

        // wait
        future_list.forEach(f -> {
            try {
                f.channel().closeFuture().sync();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });

        // clean up
        group_list.forEach(io.netty.channel.EventLoopGroup::shutdownGracefully);
    }
}

