package com.letsmidi.monsys.test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.nio.charset.Charset;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

import com.letsmidi.monsys.exchange.ExchangeClientConnection;
import com.letsmidi.monsys.exchange.TelnetClientConnection;
import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.DelimiterBasedFrameDecoder;
import io.netty.handler.codec.Delimiters;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.codec.string.StringDecoder;
import io.netty.handler.codec.string.StringEncoder;

public class ClientTest {

    public static void main(String[] args) {
        // ClientTest client = new ClientTest("127.0.0.1", 1986);
        // client.start();
        NioEventLoopGroup worker = new NioEventLoopGroup();

        try {
            //testManualConnect("127.0.0.1", 1983, worker);
            //TelnetClientConnection connection = new TelnetClientConnection(worker);
            ExchangeClientConnection connection = new ExchangeClientConnection(worker);
            ChannelFuture future = connection.connect("127.0.0.1", 1987);
            future.get();
            if (!future.isSuccess()) {
                log("failed");
            } else {
                log("connected");

                BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

                int type = Integer.valueOf(reader.readLine().trim());

                log("begin to auth");
                ChannelFuture result_future = connection.auth("1", type);

                log("waiting...");
                result_future.sync();

                boolean result = result_future.isSuccess();

                log("result: " + result);

                TelnetClientConnection telnet_connection = new TelnetClientConnection(connection.group(), connection.channel());
                for (String line = reader.readLine(); line != null && !line.equals("exit"); line = reader.readLine()) {
                    // connection.channel().writeAndFlush(line);
                    telnet_connection.channel().writeAndFlush(line);
                }

                connection.channel().close().sync();
            }
        } catch (ExecutionException | InterruptedException | IOException e) {
            e.printStackTrace();
        }

        worker.shutdownGracefully();
    }

    private static void testManualConnect(String host, int port, NioEventLoopGroup worker)
            throws ExecutionException, InterruptedException {

        Channel channel = new NioSocketChannel();
        ChannelPipeline pipeline = channel.pipeline();
        pipeline.addLast(new DelimiterBasedFrameDecoder(8192, Delimiters.lineDelimiter()));
        pipeline.addLast(new StringDecoder(Charset.forName("utf-8")));
        pipeline.addLast(new StringEncoder(Charset.forName("utf-8")));
        pipeline.addLast(new SimpleChannelInboundHandler<String>() {
            @Override
            public void channelRegistered(ChannelHandlerContext ctx) throws Exception {
                super.channelRegistered(ctx);
                log("channelRegistered");
            }

            @Override
            public void channelUnregistered(ChannelHandlerContext ctx) throws Exception {
                super.channelUnregistered(ctx);
                log("channelUnregistered");
            }

            @Override
            public void channelActive(ChannelHandlerContext ctx) throws Exception {
                super.channelActive(ctx);
                log("channelActive");
            }

            @Override
            public void channelInactive(ChannelHandlerContext ctx) throws Exception {
                super.channelInactive(ctx);
                log("channelInactive");
            }

            @Override
            public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
                super.exceptionCaught(ctx, cause);
                log("exceptionCaught");
                cause.printStackTrace();
            }

            @Override
            protected void channelRead0(ChannelHandlerContext ctx, String msg) throws Exception {
                log("from server: " + msg);
            }
        });
        channel.config().setOption(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5 * 1000);
        channel.config().setOption(ChannelOption.SO_KEEPALIVE, true);

        ChannelFuture register_future = worker.register(channel);
        register_future.get();
        if (!register_future.isSuccess()) {
            log("Failed to register channel to executor group");
            return;
        }
        ChannelPromise promise = channel.newPromise();
        channel.connect(new InetSocketAddress(host, port), promise);

        promise.get();

        log("Connected!");

        channel.closeFuture().sync();

        log("Disconnected");
    }

    private final String mRemoteHost;
    private final int mRemotePort;

    public ClientTest(String remoteHost, int remotePort) {
        mRemoteHost = remoteHost;
        mRemotePort = remotePort;
    }

    private static void log(String msg) {
        System.out.println(msg);
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
                                    new ProtobufDecoder(Client.ClientMsg.getDefaultInstance()),
                                    new ClientHandler());
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
            for (; ; ) {
                if (!ch.isActive()) {
                    log("disconnected");
                    break;
                }
                String line = reader.readLine().trim();
                System.out.println("input: [" + line + "]");
                if (line.equals("exit")) {
                    System.out.println("exit");
                    break;
                }

                if (line.length() <= 0) {
                    continue;
                }

                // last_future = ch.writeAndFlush(line + Delimiters.lineDelimiter());
                last_future = processInput(ch, line);
            }

            ch.close().sync();
            // ch.closeFuture().sync();
            if (last_future != null) {
                last_future.sync();
            }

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
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.LOGIN, 0);

        Client.Login.Builder login_builder = Client.Login.newBuilder();
        login_builder.setClientType(Client.ClientType.CLIENT_TYPE_ANDROID);
        login_builder.setUserName("zao1@gmail.com");
        login_builder.setPassword("zao1zao1no1");

        builder.setLogin(login_builder);
        return ch.writeAndFlush(builder.build());
    }

}
