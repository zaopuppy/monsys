package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.demo1.Demo1;
import com.letsmidi.monsys.protocol.demo2.Demo2;
import com.letsmidi.monsys.util.NettyUtil;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;
import io.netty.util.concurrent.FutureListener;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;
import java.util.function.Function;

/**
 * Created by zero on 6/1/15.
 */
public class InnerRouterDemoApp {
    public static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) throws InterruptedException {
        log("Just a test.");

        InnerRouterDemoApp app = new InnerRouterDemoApp();

        app.init().start();
    }

    private InnerRouterDemoApp init() {
        return this;
    }

    private void start() throws InterruptedException {
        NioEventLoopGroup shared_worker = new NioEventLoopGroup();

        ChannelFuture[] futures = new ChannelFuture[]{
            listenClients(shared_worker, 1983),
            listenAdmins(shared_worker, 1984)
        };

        for (ChannelFuture f: futures) {
            f.channel().closeFuture().sync();
        }
    }

    private static class Entry {
        private final String mClientId;
        private final Channel mChannel;

        public Entry(String clientId, Channel channel) {
            mClientId = clientId;
            mChannel = channel;
        }

        public String getClientId() {
            return mClientId;
        }

        public Channel getChannel() {
            return mChannel;
        }
    }

    private static final Map<String, Entry> mClientMap = new HashMap<>();

    private static final Map<String, Entry> mAdminMap = new HashMap<>();

    // protocol `demo1'
    private ChannelFuture listenClients(NioEventLoopGroup worker, int port) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        ChannelFuture future = NettyUtil.startServer(port, boss, worker,
            new LoggingHandler("demo1.log", LogLevel.INFO),
            new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel ch) throws Exception {
                    ch.pipeline().addLast(
                        new ProtobufVarint32LengthFieldPrepender(),
                        new ProtobufVarint32FrameDecoder(),
                        new ProtobufEncoder(),
                        new ProtobufDecoder(Demo1.DemoMsg.getDefaultInstance()),
                        new Demo1Handler());
                }
            }
        );
        return future;
    }

    private ChannelFuture listenAdmins(NioEventLoopGroup worker, int port) {
        NioEventLoopGroup boss = new NioEventLoopGroup(1);
        ChannelFuture future = NettyUtil.startServer(port, boss, worker,
            new LoggingHandler("demo2.log", LogLevel.INFO),
            new ChannelInitializer<SocketChannel>() {
                @Override
                protected void initChannel(SocketChannel ch) throws Exception {
                    ch.pipeline().addLast(
                        new ProtobufVarint32LengthFieldPrepender(),
                        new ProtobufVarint32FrameDecoder(),
                        new ProtobufEncoder(),
                        new ProtobufDecoder(Demo2.DemoMsg.getDefaultInstance()),
                        new Demo2Handler());
                }
            }
        );
        return future;
    }

    private enum State {
        WAIT_FOR_LOGIN,
        LOGGED_IN,
    }

    private static class Demo1Handler extends SimpleChannelInboundHandler<Demo1.DemoMsg> {

        private State mState = State.WAIT_FOR_LOGIN;

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, Demo1.DemoMsg msg) throws Exception {

            switch (msg.getType1()) {
                case LOGIN: {
                    log("login request");

                    if (mState == State.LOGGED_IN) {
                        // ignore
                        log("already logged in");
                        return;
                    }

                    Demo1.LoginReq1 loginReq1 = msg.getLoginReq1();

                    String clientId = loginReq1.getId1();

                    if (mClientMap.containsKey(clientId)) {
                        log("duplicated client id");
                        ctx.close();
                        return;
                    }


                    mClientMap.put(clientId, new Entry(clientId, ctx.channel()));

                    ctx.channel().closeFuture().addListener(
                        (ChannelFuture future) -> mClientMap.remove(clientId));

                    Demo1.DemoMsg.Builder builder = Demo1.DemoMsg.newBuilder();
                    builder.setType1(Demo1.MsgType.LOGIN_RSP);
                    Demo1.LoginRsp1.Builder loginRsp1 = Demo1.LoginRsp1.newBuilder();
                    loginRsp1.setCode1(0);
                    builder.setLoginRsp1(loginRsp1);

                    ctx.writeAndFlush(builder.build());

                    mState = State.LOGGED_IN;

                    log("logged in");

                    break;
                }
                case MSG: {
                    Demo1.MsgReq1 msgReq1 = msg.getMsgReq1();
                    String peerId = msgReq1.getPeerId1();
                    Entry entry = mAdminMap.getOrDefault(peerId, null);
                    if (entry == null) {
                        log("not admin is found");
                        return;
                    }

                    log("peer admin got");

                    Channel peerChannel = entry.getChannel();

                    {
                        Demo2.DemoMsg.Builder builder = Demo2.DemoMsg.newBuilder();
                        builder.setType2(Demo2.MsgType.MSG);
                        Demo2.MsgReq2.Builder msgReq2 = Demo2.MsgReq2.newBuilder();
                        msgReq2.setMsg2(msgReq1.getMsg1());
                        msgReq2.setPeerId2(msgReq1.getPeerId1());
                        builder.setMsgReq2(msgReq2);

                        peerChannel.writeAndFlush(builder.build());

                        log("msg routed");
                    }
                    break;
                }
                case MSG_RSP:
                    break;
                default:
                    break;
            }

        }
    }

    private static class Demo2Handler extends SimpleChannelInboundHandler<Demo2.DemoMsg> {

        private State mState = State.WAIT_FOR_LOGIN;

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, Demo2.DemoMsg msg) throws Exception {

            switch (msg.getType2()) {
                case LOGIN: {
                    log("login request");

                    if (mState == State.LOGGED_IN) {
                        // ignore
                        log("already logged in");
                        return;
                    }

                    Demo2.LoginReq2 loginReq2 = msg.getLoginReq2();

                    String clientId = loginReq2.getId2();

                    if (mAdminMap.containsKey(clientId)) {
                        log("duplicated client id");
                        ctx.close();
                        return;
                    }

                    mAdminMap.put(clientId, new Entry(clientId, ctx.channel()));

                    ctx.channel().closeFuture().addListener(
                        (ChannelFuture future) -> mAdminMap.remove(clientId));

                    Demo2.DemoMsg.Builder builder = Demo2.DemoMsg.newBuilder();
                    builder.setType2(Demo2.MsgType.LOGIN_RSP);
                    Demo2.LoginRsp2.Builder loginRsp2 = Demo2.LoginRsp2.newBuilder();
                    loginRsp2.setCode2(0);
                    builder.setLoginRsp2(loginRsp2);

                    ctx.writeAndFlush(builder.build());

                    mState = State.LOGGED_IN;

                    log("logged in");
                    break;
                }
                case MSG: {
                    Demo2.MsgReq2 msgReq2 = msg.getMsgReq2();
                    String peerId = msgReq2.getPeerId2();
                    Entry entry = mClientMap.getOrDefault(peerId, null);
                    if (entry == null) {
                        log("not push client is found");
                        return;
                    }

                    log("peer client got");

                    Channel peerChannel = entry.getChannel();

                    {
                        Demo1.DemoMsg.Builder builder = Demo1.DemoMsg.newBuilder();
                        builder.setType1(Demo1.MsgType.MSG);
                        Demo1.MsgReq1.Builder msgReq1 = Demo1.MsgReq1.newBuilder();
                        msgReq1.setMsg1(msgReq2.getMsg2());
                        msgReq1.setPeerId1(msgReq2.getPeerId2());
                        builder.setMsgReq1(msgReq1);

                        peerChannel.writeAndFlush(builder.build());

                        log("msg routed");
                    }

                    break;
                }
                case MSG_RSP:
                    break;
                default:
                    break;
            }

        }
    }
}
