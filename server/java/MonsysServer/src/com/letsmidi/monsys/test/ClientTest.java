package com.letsmidi.monsys.test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import com.letsmidi.monsys.protocol.client.Client;
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

public class ClientTest {
    public static void main(String[] args) {
        ClientTest client = new ClientTest("127.0.0.1", 1986);
        client.start();
    }

    private final String mRemoteHost;
    private final int mRemotePort;

    public ClientTest(String remoteHost, int remotePort) {
        mRemoteHost = remoteHost;
        mRemotePort = remotePort;
    }

    private void log(String msg) {
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

        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            worker.shutdownGracefully();
        }
    }

    private ChannelFuture processInput(Channel ch, String line) {
        System.out.println("processInput");
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.LOGIN);

        Client.Login.Builder login_builder = Client.Login.newBuilder();
        login_builder.setClientType(Client.ClientType.CLIENT_TYPE_ANDROID);
        login_builder.setUserName("zao1@gmail.com");
        login_builder.setPassword("zao1zao1no1");

        builder.setLogin(login_builder);
        return ch.writeAndFlush(builder.build());
    }

}
