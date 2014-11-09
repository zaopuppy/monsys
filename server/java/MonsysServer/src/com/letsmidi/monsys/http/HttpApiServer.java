package com.letsmidi.monsys.http;

import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.http.HttpServerCodec;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;


/**
 *
 * /account/register
 * /account/login
 * /account/get_fgw_list
 *
 *
 * Created by zero on 8/23/14.
 */
public class HttpApiServer {
  public static void main(String[] args) {
    HttpApiServer server = new HttpApiServer();
    server.start();
  }

  public void start() {
    //
    NioEventLoopGroup boss = new NioEventLoopGroup();
    NioEventLoopGroup worker = new NioEventLoopGroup();

    ChannelFuture future = NettyUtil.startServer(
        8088, boss, worker,
        new LoggingHandler(LogLevel.INFO),
        new ChannelInitializer<SocketChannel>() {
          @Override
          protected void initChannel(SocketChannel ch) throws Exception {
            ch.pipeline().addLast(
                new HttpServerCodec(),
                new HttpApiServerHandler()
            );
          }
        });

    try {
      future.channel().closeFuture().sync();
    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      boss.shutdownGracefully();
      worker.shutdownGracefully();
    }
  }
}
