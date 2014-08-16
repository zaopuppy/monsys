package com.letsmidi.monsys.push;

import com.letsmidi.monsys.log.MyLogFormatter;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
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

import java.io.IOException;
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
  public static final String LOGGER_NAME = "push-server";
  private static final String LOG_FILE_NAME = "pushserver.log";

  private static final int PUSH_PORT = 1984;
  private static final int ACCESS_PORT = 1988;

  private final Logger mLogger = Logger.getLogger(PushServer.LOGGER_NAME);

  public static void main(String[] args) throws IOException {

    initLogger();

    PushServer push_server = new PushServer();
    push_server.start();
  }

  private static void initLogger() throws IOException {
    Handler log_handler = new FileHandler(LOG_FILE_NAME, 1 << 20, 10000, true);

    final Logger logger = Logger.getLogger(LOGGER_NAME);
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

    // push server
    NioEventLoopGroup push_boss = new NioEventLoopGroup();
    NioEventLoopGroup push_worker = new NioEventLoopGroup();

    ChannelFuture push_future = NettyUtil.startServer(
        PUSH_PORT, push_boss, push_worker,
        new LoggingHandler(LOGGER_NAME, LogLevel.INFO),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(PushMsg.getDefaultInstance()),
                new PushServerHandler());
          }
        }
    );

    // access server
    NioEventLoopGroup access_boss = new NioEventLoopGroup();
    NioEventLoopGroup access_worker = new NioEventLoopGroup();

    ChannelFuture access_future = NettyUtil.startServer(
        ACCESS_PORT, access_boss, access_worker,
        new LoggingHandler(LOGGER_NAME, LogLevel.INFO),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(PushMsg.getDefaultInstance()),
                new AccessServerHandler());
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

    //ChannelFuture push_future = startPushServer();
    //ChannelFuture ctrl_future = startAccessServer();
    //
    //try {
    //  push_future.channel().closeFuture().sync();
    //  ctrl_future.channel().closeFuture().sync();
    //} catch (InterruptedException e) {
    //  e.printStackTrace();
    //}
  }

//  public ChannelFuture startPushServer() {
//    mLogger.info("startPushServer()");
//
//    ServerBootstrap b = new ServerBootstrap();
//    NioEventLoopGroup boss = new NioEventLoopGroup();
//    NioEventLoopGroup worker = new NioEventLoopGroup();
//
//    try {
//      b.group(boss, worker)
//       .channel(NioServerSocketChannel.class)
//       .handler(new LoggingHandler(LOGGER_NAME, LogLevel.INFO))
//       .childHandler(new ChannelInitializer<SocketChannel>() {
//        @Override
//        protected void initChannel(SocketChannel ch) throws Exception {
//          ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
//                                new ProtobufVarint32FrameDecoder(),
//                                new ProtobufEncoder(),
//                                new ProtobufDecoder(PushMsg.getDefaultInstance()),
//                                new PushServerHandler());
//        }
//       })
//       .option(ChannelOption.SO_BACKLOG, 128)
//       .childOption(ChannelOption.SO_KEEPALIVE, true);
//
//      return b.bind(PUSH_PORT).sync();
//      // future.channel().closeFuture().sync();
//    } catch (InterruptedException e) {
//      e.printStackTrace();
////    } finally {
////      worker.shutdownGracefully();
////      boss.shutdownGracefully();
////    }
//    }
//
//    return null;
//  }
//
//  public ChannelFuture startAccessServer() {
//    mLogger.info("startAccessServer()");
//
//    ServerBootstrap b = new ServerBootstrap();
//    NioEventLoopGroup boss = new NioEventLoopGroup();
//    NioEventLoopGroup worker = new NioEventLoopGroup(1);
//
//    try {
//      b.group(boss, worker)
//      .channel(NioServerSocketChannel.class)
//      .handler(new LoggingHandler(LogLevel.DEBUG))
//      .childHandler(new ChannelInitializer<SocketChannel>() {
//        @Override
//        protected void initChannel(SocketChannel ch) throws Exception {
////          ch.pipeline().addLast(new DelimiterBasedFrameDecoder(1024, Delimiters.lineDelimiter()),
////                                new StringDecoder(), new StringEncoder(),
////                                new CtrlServerHandler());
//          ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
//                                new ProtobufVarint32FrameDecoder(),
//                                new ProtobufEncoder(),
//                                new ProtobufDecoder(PushMsg.getDefaultInstance()),
//                                new AccessServerHandler());
//        }
//      })
//      .option(ChannelOption.SO_BACKLOG, 16)
//      .childOption(ChannelOption.SO_KEEPALIVE, true);
//
//      return b.bind(ACCESS_PORT).sync();
//    } catch (InterruptedException e) {
//      e.printStackTrace();
//    }
//
//    return null;
//  }

}
