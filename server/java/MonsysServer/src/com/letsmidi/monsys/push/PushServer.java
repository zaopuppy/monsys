package com.letsmidi.monsys.push;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.log.MyLogFormatter;
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

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

// TODO: android client logged in, then close the screen, wait several minutes, then open screen, we'll find nothing
//       gonna happen if we invoke server interface
//  case1: another connection comes while there's already one connection is established
//  case2: for some reason, channel didn't closed even the peer channel has been closed
//  solution: clean and disconnect old connection if new one is coming
// TODO: heartbeat
public class PushServer {
  private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

  public static void main(String[] args) throws IOException, MonsysException {

    Config.load();

    initLogger();

    PushServer push_server = new PushServer();
    push_server.start();
  }

  private static void initLogger() throws IOException {
    Handler log_handler = new FileHandler(Config.getPushConfig().getLogFileName(), 1 << 20, 10000, true);

    final Logger logger = Logger.getLogger(Config.getPushConfig().getLoggerName());
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

  public void start() {
    mLogger.info("-----------------------------------");
    mLogger.info("push server start");

    Class[] mapping_classes = new Class[] {
        AccountInfo.class,
    };

    // initialize hiberate
    if (!HibernateUtil.init(mapping_classes)) {
      mLogger.severe("Failed to initialize hiberate, failed");
      return;
    }

    // global timer
    final HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
    timer.start();

    // push server
    NioEventLoopGroup push_boss = new NioEventLoopGroup();
    NioEventLoopGroup push_worker = new NioEventLoopGroup();

    ChannelFuture push_future = NettyUtil.startServer(
        Config.getPushConfig().getPushPort(), push_boss, push_worker,
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

    // access server
    NioEventLoopGroup access_boss = new NioEventLoopGroup();
    NioEventLoopGroup access_worker = new NioEventLoopGroup();

    ChannelFuture access_future = NettyUtil.startServer(
        Config.getPushConfig().getAccessPort(), access_boss, access_worker,
        new LoggingHandler(Config.getPushConfig().getLoggerName(), LogLevel.INFO),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(
                new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(PushMsg.getDefaultInstance()),
                new AccessServerHandler(timer));
          }
        }
    );

    try {
      push_future.channel().closeFuture().sync();
      access_future.channel().closeFuture().sync();
    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      access_boss.shutdownGracefully();
      access_worker.shutdownGracefully();
    }

  }
}

