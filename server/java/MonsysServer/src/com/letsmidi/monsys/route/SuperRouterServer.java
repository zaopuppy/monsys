package com.letsmidi.monsys.route;

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.log.LogFormatter;
import com.letsmidi.monsys.log.Logger;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.session.SessionManager;
import com.letsmidi.monsys.util.MonsysException;
import com.letsmidi.monsys.util.NettyUtil;
import com.letsmidi.monsys.util.ZookeeperUtil;
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


/**
 * Token allocated by Control module, usually associated with account/password/expired-time.
 * <p>
 * SuperRoute is just a Route, do nothing but routing.
 */
public class SuperRouterServer {
    private static final String TAG = "SuperRouterServer";

    public SuperRouterServer() {
    }

    private void start() {
        Logger.i(TAG, "starting route & access server...");

        // global timer
        HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
        timer.start();

        // global session manager
        final SessionManager session_manager = new SessionManager(timer);

        // for accepting routing module
        NioEventLoopGroup route_boss = new NioEventLoopGroup();
        NioEventLoopGroup route_worker = new NioEventLoopGroup();

        ChannelFuture route_future = NettyUtil.startServer(
                Config.getRouterConfig().getAccessPort(), route_boss, route_worker,
                new LoggingHandler(LogLevel.DEBUG),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                                new RouterAccessServerHandler(session_manager));
                    }
                });

        // for allocating routing token
        NioEventLoopGroup access_boss = new NioEventLoopGroup();
        NioEventLoopGroup access_worker = new NioEventLoopGroup();

        ChannelFuture access_future = NettyUtil.startServer(
                Config.getRouterConfig().getAllocatePort(), route_boss, route_worker,
                new LoggingHandler(LogLevel.DEBUG),
                new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) throws Exception {
                        ch.pipeline().addLast(
                                new ProtobufVarint32LengthFieldPrepender(),
                                new ProtobufVarint32FrameDecoder(),
                                new ProtobufEncoder(),
                                new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                                new RouterAllocateServerHandler(session_manager));
                    }
                });

        if (!notifyNameServer()) {
            Logger.e(TAG, "Failed to notify name server, shutting down");
            route_future.channel().close();
            access_future.channel().close();
        }

        try {
            route_future.channel().closeFuture().sync();
            access_future.channel().closeFuture().sync();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            route_boss.shutdownGracefully();
            route_worker.shutdownGracefully();
            access_boss.shutdownGracefully();
            access_worker.shutdownGracefully();
        }
    }

    public static class SuperRouterServiceInfo {
        private String mHost = null;
        private int mAllocatePort = -1;
        private int mAccessPort = -1;

        public String getHost() {
            return mHost;
        }

        public void setHost(String host) {
            mHost = host;
        }

        public int getAllocatePort() {
            return mAllocatePort;
        }

        public void setAllocatePort(int port) {
            mAllocatePort = port;
        }

        public int getAccessPort() {
            return mAccessPort;
        }

        public void setAccessPort(int port) {
            mAccessPort = port;
        }

        public boolean isValid() {
            return mHost != null && mAllocatePort > 0 && mAccessPort > 0;
        }

        public byte[] encode() {
            StringBuilder builder = new StringBuilder(1024);
            builder.append(mHost).append('|')
                    .append(mAllocatePort).append('|')
                    .append(mAccessPort);
            return builder.toString().getBytes();
        }

        public boolean decode(byte[] data) {
            if (data == null || data.length <= 0) {
                return false;
            }

            String[] fields = new String(data).split("\\|");
            if (fields.length != 3) {
                return false;
            }

            setHost(fields[0]);
            setAllocatePort(Integer.parseInt(fields[1]));
            setAccessPort(Integer.parseInt(fields[2]));

            return true;
        }
    }

    private boolean notifyNameServer() {
        if (!ZookeeperUtil.init(
                Config.getCommonConfig().getZookeeperConnectString(),
                Config.getCommonConfig().getZookeeperConnectTimeout())) {
            return false;
        }

        SuperRouterServiceInfo info = new SuperRouterServiceInfo();
        info.setHost(Config.getCommonConfig().getHostName());
        info.setAllocatePort(Config.getRouterConfig().getAllocatePort());
        info.setAccessPort(Config.getRouterConfig().getAccessPort());

        return ZookeeperUtil.register(
                Config.getCommonConfig().getZookeeperNameServicePath()
                        + '/' + Config.getRouterConfig().getNameServicePath(),
                Config.getRouterConfig().getNameServiceNode(),
                info.encode());
    }

    public static void main(String[] args) throws IOException, MonsysException {
        Config.load();

        initLogger();

        SuperRouterServer router = new SuperRouterServer();
        router.start();
    }

    private static void initLogger() throws IOException {
        Handler log_handler = new FileHandler(Config.getRouterConfig().getLogFileName(), 1 << 20, 10000, true);

        final java.util.logging.Logger logger = java.util.logging.Logger.getLogger(Config.getRouterConfig().getLoggerName());
        logger.setLevel(Level.ALL);
        logger.addHandler(log_handler);

        for (Handler h : logger.getHandlers()) {
            System.out.println("handler: " + h.getClass().getCanonicalName());
            h.setFormatter(new LogFormatter());
        }

        //InternalLoggerFactory.setDefaultFactory(new InternalLoggerFactory() {
        //  @Override
        //  protected InternalLogger newInstance(String name) {
        //    return new JdkLogger(logger);
        //  }
        //});

        //MyLogger.setLogger(logger);
    }


}
