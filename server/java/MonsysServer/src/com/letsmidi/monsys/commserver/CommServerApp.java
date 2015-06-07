package com.letsmidi.monsys.commserver;

import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.letsmidi.monsys.database.AccountInfo;
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
    private static final String TAG = "CommServerApp";

    public static void main(String[] args) throws IOException, MonsysException {

        initLogger();

        Class[] mapping_classes = new Class[]{
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
        //Handler log_handler = new FileHandler(CommConfig.LoggerFileName, 1 << 20, 10000, true);
        //
        //final Logger logger = Logger.getLogger(CommConfig.LoggerName);
        //logger.setLevel(Level.ALL);
        //logger.addHandler(log_handler);
        //
        //for (Handler h : logger.getHandlers()) {
        //    System.out.println("handler: " + h.getClass().getCanonicalName());
        //    h.setFormatter(new MyLogFormatter());
        //}

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

        future_list.add(connectLoginServer(shared_worker));

        future_list.add(listenForClients(shared_worker));

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

    private ChannelFuture listenForClients(NioEventLoopGroup worker) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        ChannelFuture future = NettyUtil.startServer(
                CommConfig.ClientListenPort, boss, worker,
                new LoggingHandler(CommConfig.LoggerName, LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(Client.ClientMsg.getDefaultInstance())/*,
                                new ClientHandler(timer)*/);
                    }
                }
        );
        return future;
    }

    // TODO: should i use MQ instead of this?
    private ChannelFuture connectLoginServer(NioEventLoopGroup worker) {
        LoginServerConnection connection = new LoginServerConnection(worker);
        return connection.connect("127.0.0.1", 1984);
    }
}

