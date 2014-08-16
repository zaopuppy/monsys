package com.letsmidi.monsys.center;

import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.center.Center;
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


/**
 *
 *
 */
public class CenterServer {
  public static final int PORT = 4433;

  public CenterServer() {
  }

  private void start() {
    MyLogger.i("starting route & access server...");

    // for accepting routing module
    NioEventLoopGroup center_boss = new NioEventLoopGroup();
    NioEventLoopGroup center_worker = new NioEventLoopGroup();

    ChannelFuture route_future = NettyUtil.startServer(
        PORT, center_boss, center_worker,
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

    if (!registerZookeeper()) {
      route_future.channel().close();
    }

    try {
      route_future.channel().closeFuture().sync();
    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      center_boss.shutdownGracefully();
      center_worker.shutdownGracefully();
    }
  }

  private boolean registerZookeeper() {
    // if (!ZookeeperUtil.init())
    // ZookeeperUtil.close();
    return false;
  }

  public static void main(String[] args) {
    CenterServer route = new CenterServer();
    route.start();
  }

}
