package com.letsmidi.monsys.login;

import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.log.MyLogFormatter;
import com.letsmidi.monsys.protocol.client.Client.ClientMsg;
import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MonsysException;
import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.util.HashedWheelTimer;


/**
 * 开发的时候，阅读参考了某公司的开源服务端代码，有如下问题：
 *
 * 1. 服务端和客户端共用一个监听，根据上报的类型区分服务端和客户端，很容易欺骗伪装为服务端，从而得到客户端的隐私（用户名，密码，聊天记录）
 *    -- 分开监听
 *
 * 2. 流程上是先申请服务端再进行登录，也就是说可以在没有用户名和密码的情况下就能枚举所有服务端
 *    -- 也许做两次验证(login & msg server各一次)可以解决这个问题?
 *
 *
 *
 */
public class LoginServerApp {
    private final Logger mLogger = Logger.getLogger(LoginConfig.LoggerName);

    public static void main(String[] args) throws IOException, MonsysException {

        //Config.load();

        initLogger();

        Class[] mapping_classes = new Class[] {
                AccountInfo.class,
        };

        // initialize hibernate
        if (!HibernateUtil.init(mapping_classes)) {
            System.out.println("Failed to initialize hibernate, failed");
            return;
        }


        LoginServerApp push_server = new LoginServerApp();
        push_server.start();
    }

    private static void initLogger() throws IOException {
        Handler log_handler = new FileHandler(LoginConfig.LoggerFileName, 1 << 20, 10000, true);

        final Logger logger = Logger.getLogger(LoginConfig.LoggerName);
        logger.setLevel(Level.ALL);
        logger.addHandler(log_handler);

        for (Handler h: logger.getHandlers()) {
            System.out.println("handler: " + h.getClass().getCanonicalName());
            h.setFormatter(new MyLogFormatter());
        }

    }

    public void start() {
        mLogger.info("-----------------------------------");
        mLogger.info("login server start");

        // global timer
        final HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
        timer.start();

        ArrayList<EventLoopGroup> group_list = new ArrayList<>();
        ArrayList<ChannelFuture> future_list = new ArrayList<>();

        // share worker
        NioEventLoopGroup shared_worker = new NioEventLoopGroup();
        group_list.add(shared_worker);

        // listen clients
        NioEventLoopGroup client_boss = new NioEventLoopGroup(1);
        ChannelFuture client_future = NettyUtil.startServer(
                LoginConfig.ClientListenPort, client_boss, shared_worker,
                new LoggingHandler(LoginConfig.LoggerName, LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(ClientMsg.getDefaultInstance()),
                                new ClientHandler(timer));
                    }
                }
        );
        group_list.add(client_boss);
        future_list.add(client_future);

        // listen servers
        NioEventLoopGroup msg_server_boss = new NioEventLoopGroup(1);
        ChannelFuture msg_server_future = NettyUtil.startServer(
                LoginConfig.CommServerListenPort, msg_server_boss, shared_worker,
                new LoggingHandler(LoginConfig.LoggerName, LogLevel.INFO),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(CommServer.CommServerMsg.getDefaultInstance()),
                                new CommServerHandler(timer));
                    }
                }
        );
        group_list.add(msg_server_boss);
        future_list.add(msg_server_future);

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

