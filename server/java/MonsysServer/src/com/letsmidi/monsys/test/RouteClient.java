package com.letsmidi.monsys.test;

import com.letsmidi.monsys.log.MyLogger;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.Channel;
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class RouteClient {
  public static void main(String[] args) {
    if (args.length != 1) {
      MyLogger.e("Usage: xxx <port>");
      return;
    }

    RouteClient push_client = new RouteClient("127.0.0.1", Integer.parseInt(args[0]));
    push_client.start();
  }

  private final String mRemoteHost;
  private final int mRemotePort;

  public RouteClient(String remoteHost, int remotePort) {
    mRemoteHost = remoteHost;
    mRemotePort = remotePort;
  }

  public void start() {
    Bootstrap b = new Bootstrap();
    NioEventLoopGroup worker = new NioEventLoopGroup(1);

    try {
      b.group(worker)
          .channel(NioSocketChannel.class)
          .handler(new ChannelInitializer<SocketChannel>() {
            @Override
            protected void initChannel(SocketChannel ch) throws Exception {
              ch.pipeline().addLast(
                  new ProtobufVarint32LengthFieldPrepender(),
                  new ProtobufVarint32FrameDecoder(),
                  new ProtobufEncoder(),
                  new ProtobufDecoder(Route.RouteMsg.getDefaultInstance()),
                  new RouteClientHandler());
            }
          })
          .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 60 * 1000)
          .option(ChannelOption.SO_KEEPALIVE, true);

      // connect to server
      final int connection_count = 1;
      ChannelFuture[] futures = new ChannelFuture[connection_count];
      for (int i = 0; i < connection_count; ++i) {
        System.out.println(i);
        futures[i] = b.connect(mRemoteHost, mRemotePort);
      }

      // wait util established
      Channel[] channels = new Channel[connection_count];
      for (int i = 0; i < connection_count; ++i) {
        channels[i] = futures[i].sync().channel();
      }

      System.out.println("done for " + connection_count + " connections");

      // use the 1st connection
      Channel ch = channels[0];
      ChannelFuture last_future = null;
      BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
      for (;;) {
        String line = reader.readLine();
        System.out.println("input: [" + line + "]");
        if (line == null || line.equals("exit")) {
          System.out.println("exit");
          break;
        }

        line = line.trim();

        if (line.length() <= 0) {
          continue;
        }

        // last_future = ch.writeAndFlush(line + Delimiters.lineDelimiter());
        last_future = processInput(ch, line);
      }

      if (last_future != null) {
        last_future.sync();
      }

      ch.close().sync();

      for (Channel channel : channels) {
        channel.close().sync();
        // ch.closeFuture().wait();
      }

    } catch (InterruptedException | IOException e) {
      e.printStackTrace();
    } finally {
      worker.shutdownGracefully();
    }
  }

  private ChannelFuture processInput(Channel ch, String line) {
    System.out.println("processInput");

    final String token = "ae@#dawz";

    if (line.startsWith("request")) {
      Route.RouteMsg.Builder builder = MsgUtil.newRouteMsgBuilder(Route.MsgType.REQ_TOKEN);

      Route.ReqToken.Builder req_token = Route.ReqToken.newBuilder();
      req_token.setToken(token);
      req_token.setAddrType(Route.AddrType.IP_V4);
      req_token.setClientIpV4(0);
      req_token.setFgwIpV4(0);

      builder.setReqToken(req_token);

      return ch.writeAndFlush(builder.build());
    } else if (line.equals("token")) {
      System.out.println("token: " + token);
    } else if (line.startsWith("connect")) {
      Route.RouteMsg.Builder builder = MsgUtil.newRouteMsgBuilder(Route.MsgType.CONNECT);

      Route.Connect.Builder connect = Route.Connect.newBuilder();
      connect.setToken(token);
      if (line.contains("client")) {
        connect.setClientType(Route.ClientType.CLIENT);
      } else {
        connect.setClientType(Route.ClientType.FGW);
      }

      builder.setConnect(connect);

      return ch.writeAndFlush(builder.build());
    } else {
      Route.RouteMsg.Builder builder = MsgUtil.newRouteMsgBuilder(Route.MsgType.CONNECT);

      Route.Connect.Builder connect = Route.Connect.newBuilder();
      connect.setToken(line);
      connect.setClientType(Route.ClientType.CLIENT);

      builder.setConnect(connect);

      return ch.writeAndFlush(builder.build());
    }

    return null;
  }

}
