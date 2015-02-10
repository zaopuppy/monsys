package com.letsmidi.monsys.center;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.log.MyLogFormatter;
import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.center.Center;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MonsysException;
import com.letsmidi.monsys.util.NettyUtil;
import com.letsmidi.monsys.util.ZookeeperUtil;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;

import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * TODO: client auto-connect mechanism
 * TODO: use zookeeper, refresh available SuperRouter list if node changed
 */
public class CenterServer {
  private final Logger mLogger = Logger.getLogger(Config.getCenterConfig().getLoggerName());

  public CenterServer() {
  }

  /**
   *
   */
  private void start() {
    final int TIMEOUT = Config.getCenterConfig().getTimeout();

    MyLogger.i("starting route & access server...");

    // initialize hibernate
    Class[] mapping_classes = new Class[] {
        AccountInfo.class,
    };
    if (!HibernateUtil.init(mapping_classes)) {
      mLogger.severe("Failed to initialize hiberate, failed");
      return;
    }

    // a shared worker
    NioEventLoopGroup worker = new NioEventLoopGroup();

    // -------------------------------------------------------
    // for connection to push server (as CLIENT)
    Bootstrap push_client_bootstrap = new Bootstrap();
    // NioEventLoopGroup push_client_worker = new NioEventLoopGroup();
    push_client_bootstrap.group(worker)
        .channel(NioSocketChannel.class)
        .handler(new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(Push.PushMsg.getDefaultInstance()),
                new CenterPushClientHandler());
          }
        })
        .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, TIMEOUT)
        .option(ChannelOption.SO_KEEPALIVE, true);

    ChannelFuture center_push_future =
        push_client_bootstrap.connect(
            Config.getCenterConfig().getPushServerHost(),
            Config.getCenterConfig().getPushServerPort()
        );

    // TODO
    //ServiceManager.INSTANCE.register(
    //    "push",
    //    new ServiceManager.ChannelService(center_push_future.channel()));

    // -------------------------------------------------------
    // for connecting to SuperRouter (as CLIENT)
    Bootstrap router_client_bootstrap = new Bootstrap();
    router_client_bootstrap.group(worker)
        .channel(NioSocketChannel.class)
        .handler(new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                new CenterRouterClientHandler());
          }
        })
        .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, TIMEOUT)
        .option(ChannelOption.SO_KEEPALIVE, true);

    ChannelFuture center_router_future =
        router_client_bootstrap.connect(
            Config.getCenterConfig().getRouterServerHost(),
            Config.getCenterConfig().getRouterServerPort()
        );

    // TODO
    //ServiceManager.INSTANCE.register(
    //    "router",
    //    new ServiceManager.ChannelService(center_router_future.channel()));

    // -------------------------------------------------------
    // for accepting client (as SERVER)
    NioEventLoopGroup center_boss = new NioEventLoopGroup();
    ChannelFuture center_future = NettyUtil.startServer(
        Config.getCenterConfig().getAccessPort(), center_boss, worker,
        new LoggingHandler(LogLevel.DEBUG),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(
                new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(Center.CenterMsg.getDefaultInstance()),
                new CenterServerHandler());
          }
        });

    // register self to Zookeeper
    if (!registerZookeeper()) {
      mLogger.severe("Failed to register to zookeeper, shutdown");
      center_future.channel().close();
    }

    try {
      center_future.channel().closeFuture().sync();
    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      center_boss.shutdownGracefully();
      worker.shutdownGracefully();
    }
  }

  /**
   *
   */
  public static class CenterServiceInfo {
    private String mHost = null;
    private int mAccessPort = -1;

    public String getHost() { return mHost; }
    public void setHost(String host) { mHost = host; }
    public int getAccessPort() { return mAccessPort; }
    public void setAccessPort(int port) { mAccessPort = port; }

    public boolean isValid() {
      return mHost != null && mAccessPort > 0;
    }

    public byte[] encode() {
      /*
      StringBuilder builder = new StringBuilder(1024);
      builder.append(mHost).append('|')
          .append(mAccessPort);
      return builder.toString().getBytes();
      */
      return (mHost + '|' + mAccessPort).getBytes();
    }

    public boolean decode(byte[] data) {
      if (data == null || data.length <= 0) {
        return false;
      }

      String[] fields = new String(data).split("\\|");
      if (fields.length != 2) {
        return false;
      }

      setHost(fields[0]);
      setAccessPort(Integer.parseInt(fields[1]));

      return true;
    }
  }

  private boolean registerZookeeper() {
    if (!ZookeeperUtil.init(
        Config.getCommonConfig().getZookeeperConnectString(),
        Config.getCommonConfig().getZookeeperConnectTimeout())) {
      ZookeeperUtil.close();
      return false;
    }

    CenterServiceInfo info = new CenterServiceInfo();
    info.setHost(Config.getCommonConfig().getHostName());
    info.setAccessPort(Config.getCenterConfig().getAccessPort());

    return ZookeeperUtil.register(
        Config.getCommonConfig().getZookeeperNameServicePath()
            + '/' + Config.getCenterConfig().getNameServicePath(),
        Config.getCenterConfig().getNameServiceNode(),
        info.encode());
  }

  public static void main(String[] args) throws MonsysException, IOException {
    Config.load();

    // initLogger();

    CenterServer server = new CenterServer();
    server.start();
  }

  private static void initLogger() throws IOException {
    Handler log_handler = new FileHandler(Config.getCenterConfig().getLogFileName(), 1 << 20, 10000, true);

    final Logger logger = Logger.getLogger(Config.getCenterConfig().getLoggerName());
    logger.setLevel(Level.ALL);
    logger.addHandler(log_handler);

    for (Handler h: logger.getHandlers()) {
      System.out.println("handler: " + h.getClass().getCanonicalName());
      h.setFormatter(new MyLogFormatter());
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
