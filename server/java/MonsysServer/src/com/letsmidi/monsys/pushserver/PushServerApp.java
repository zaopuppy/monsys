package com.letsmidi.monsys.pushserver;

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.database.DeviceInfo;
import com.letsmidi.monsys.log.LogFormatter;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MonsysException;
import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.util.HashedWheelTimer;

// TODO: android client logged in, then close the screen, wait several minutes, then open screen, we'll find nothing
//       gonna happen if we invoke server interface
//  case1: another connection comes while there's already one connection is established
//  case2: for some reason, channel didn't closed even the peer channel has been closed
//  solution: clean and disconnect old connection if new one is coming
// TODO: heartbeat
public class PushServerApp {
    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

    public static void main(String[] args) throws IOException, MonsysException {

        Config.load();

        initLogger();

        PushServerApp push_server = new PushServerApp();
        push_server.start();
    }

    private static void initLogger() throws IOException {
        Handler log_handler = new FileHandler(Config.getPushConfig().getLogFileName(), 1 << 20, 10000, true);

        final Logger logger = Logger.getLogger(Config.getPushConfig().getLoggerName());
        logger.setLevel(Level.ALL);
        logger.addHandler(log_handler);

        for (Handler h : logger.getHandlers()) {
            System.out.println("handler: " + h.getClass().getCanonicalName());
            h.setFormatter(new LogFormatter());
        }
    }

    public void start() {
        mLogger.info("-----------------------------------");
        mLogger.info("push server start");

        Class[] mapping_classes = new Class[]{
            AccountInfo.class,
            DeviceInfo.class,
        };

        // initialize hibernate
        if (!HibernateUtil.init(mapping_classes)) {
            mLogger.severe("Failed to initialize hibernate, failed");
            return;
        }

        // global timer
        final HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
        timer.start();

        InMemInfo.INSTANCE.init(timer);

        NioEventLoopGroup shared_worker = new NioEventLoopGroup();

        ChannelFuture[] futures = new ChannelFuture[]{
            listenPushClients(shared_worker, Config.getPushConfig().getPushPort(), timer),
            listenApiClients(shared_worker, Config.getPushConfig().getApiPort(), timer)
        };

        try {
            for (ChannelFuture f: futures) {
                f.channel().closeFuture().sync();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            //push_boss.shutdownGracefully();
            //access_boss.shutdownGracefully();
            //shared_worker.shutdownGracefully();
        }

    }

    /**
     * 监听push客户端
     *
     * @param worker
     * @param port
     * @param timer
     * @return
     */
    private ChannelFuture listenPushClients(NioEventLoopGroup worker, int port, HashedWheelTimer timer) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        ChannelFuture future = NettyUtil.startServer(port, boss, worker,
            new LoggingHandler(Config.getPushConfig().getLoggerName(), LogLevel.INFO),
            new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel ch) throws Exception {
                    ch.pipeline().addLast(
                        new ProtobufVarint32LengthFieldPrepender(),
                        new ProtobufVarint32FrameDecoder(),
                        new ProtobufEncoder(),
                        new ProtobufDecoder(PushMsg.getDefaultInstance()),
                        new PushServerHandler(timer));
                }
            }
        );
        return future;
    }

    /**
     * 监听内部接入客户端, 用于下发请求给客户端
     *
     * @param worker
     * @param port
     * @param timer
     * @return
     */
    private ChannelFuture listenApiClients(NioEventLoopGroup worker, int port, HashedWheelTimer timer) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        ChannelFuture future = NettyUtil.startServer(port, boss, worker,
            new LoggingHandler(Config.getPushConfig().getLoggerName(), LogLevel.INFO),
            new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel ch) throws Exception {
                    ch.pipeline().addLast(
                        new ProtobufVarint32LengthFieldPrepender(),
                        new ProtobufVarint32FrameDecoder(),
                        new ProtobufEncoder(),
                        new ProtobufDecoder(PushMsg.getDefaultInstance()),
                        new ApiServerHandler(timer));
                }
            }
        );
        return future;
    }
}

