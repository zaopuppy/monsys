package com.letsmidi.monsys.center;

import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.route.RouteAccessServerHandler;
import com.letsmidi.monsys.route.RouteAllocateServerHandler;
import com.letsmidi.monsys.route.session.RouteSession;
import com.letsmidi.monsys.route.session.SessionManager;
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

import java.util.concurrent.TimeUnit;


/**
 *
 *
 */
public class Center {
  public static final int ACCESS_PORT = 4444;
  public static final int ALLOCATE_PORT = 4445;

  public Center() {
  }

  private void start() {
    MyLogger.i("starting route & access server...");

    // global timer
    HashedWheelTimer timer = new HashedWheelTimer(1, TimeUnit.SECONDS);
    timer.start();

    // global session manager
    final SessionManager session_manager = new SessionManager(timer);

    // for accepting routing module
    NioEventLoopGroup route_boss = new NioEventLoopGroup();
    NioEventLoopGroup route_worker = new NioEventLoopGroup();

    ChannelFuture route_future = NettyUtil.startServer(
        ACCESS_PORT, route_boss, route_worker,
        new LoggingHandler(LogLevel.DEBUG),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(
                new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                new RouteAccessServerHandler(session_manager));
          }
        });

    // for allocating routing token
    NioEventLoopGroup access_boss = new NioEventLoopGroup();
    NioEventLoopGroup access_worker = new NioEventLoopGroup();

    ChannelFuture access_future = NettyUtil.startServer(
        ALLOCATE_PORT, route_boss, route_worker,
        new LoggingHandler(LogLevel.DEBUG),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(
                new ProtobufVarint32LengthFieldPrepender(),
                new ProtobufVarint32FrameDecoder(),
                new ProtobufEncoder(),
                new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                new RouteAllocateServerHandler(session_manager));
          }
        });

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

  public static void main(String[] args) {
    Center route = new Center();
    route.start();
  }

}
