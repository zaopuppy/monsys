package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.demo1.Demo1;
import com.letsmidi.monsys.protocol.demo2.Demo2;
import com.letsmidi.monsys.util.NettyUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;

import java.util.HashMap;
import java.util.Map;

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
        private final String clientId;
        private final Channel channel;

        public Entry(String clientId, Channel channel) {
            this.clientId = clientId;
            this.channel = channel;
        }

        public String getClientId() {
            return clientId;
        }

        public Channel getChannel() {
            return channel;
        }
    }

    private static final SequenceGenerator mSeqGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private static final Map<String, Entry> mClientMap = new HashMap<>();

    private static final Map<String, Entry> mAdminMap = new HashMap<>();

    private static final Map<Integer, RouteRecord> mRouteMap = new HashMap<>(10000);
    private static class RouteRecord {
        private final int innerSeq;
        private final int externalSeq;
        private final String peerId;

        public RouteRecord(int innerSeq, int externalSeq, String peerId) {
            this.innerSeq = innerSeq;
            this.externalSeq = externalSeq;
            this.peerId = peerId;
        }

        public int getInnerSeq() {
            return innerSeq;
        }

        public int getExternalSeq() {
            return externalSeq;
        }

        public String getPeerId() {
            return peerId;
        }
    }

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

    private static int genSeq() {
        synchronized (mSeqGenerator) {
            return mSeqGenerator.next();
        }
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
                    handleLogin(ctx, msg);
                    break;
                }
                case MSG_RSP:
                    handleMsgRsp(ctx, msg);
                    break;
                default:
                    break;
            }

        }

        private void handleMsgRsp(ChannelHandlerContext ctx, Demo1.DemoMsg msg) {
            if (!msg.hasMsgRsp1()) {
                log("bad response");
                return;
            }

            final RouteRecord record = mRouteMap.remove(msg.getSeq1());
            if (record == null) {
                log("no route record");
                return;
            }

            Entry entry = mAdminMap.getOrDefault(record.peerId, null);
            if (entry == null) {
                log("not admin is found");
                return;
            }

            log("peer admin got");

            Demo1.MsgRsp1 msgRsp1 = msg.getMsgRsp1();

            final Demo2.DemoMsg convertedMsg;
            {
                Demo2.DemoMsg.Builder builder = Demo2.DemoMsg.newBuilder();
                builder.setType2(Demo2.MsgType.MSG_RSP);
                builder.setSeq2(record.getExternalSeq());

                Demo2.MsgRsp2.Builder msgRsp2 = Demo2.MsgRsp2.newBuilder();
                msgRsp2.setCode2(msgRsp1.getCode1());
                builder.setMsgRsp2(msgRsp2);

                convertedMsg = builder.build();

                log("msg routed");
            }

            Channel peerChannel = entry.getChannel();
            peerChannel.writeAndFlush(convertedMsg);

            log("msg routed");
        }

        private void handleLogin(ChannelHandlerContext ctx, Demo1.DemoMsg msg) {
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
            builder.setSeq1(msg.getSeq1());
            Demo1.LoginRsp1.Builder loginRsp1 = Demo1.LoginRsp1.newBuilder();
            loginRsp1.setCode1(0);
            builder.setLoginRsp1(loginRsp1);

            ctx.writeAndFlush(builder.build());

            mState = State.LOGGED_IN;

            log("logged in");
        }
    }

    private static class Demo2Handler extends SimpleChannelInboundHandler<Demo2.DemoMsg> {

        private State mState = State.WAIT_FOR_LOGIN;
        private String mClientId = null;

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
                    builder.setSeq2(msg.getSeq2());
                    Demo2.LoginRsp2.Builder loginRsp2 = Demo2.LoginRsp2.newBuilder();
                    loginRsp2.setCode2(0);
                    builder.setLoginRsp2(loginRsp2);

                    ctx.writeAndFlush(builder.build());

                    mClientId = clientId;

                    mState = State.LOGGED_IN;

                    log("logged in");
                    break;
                }
                case MSG: {
                    if (mState != State.LOGGED_IN) {
                        log("not logged in yet");
                        return;
                    }

                    Demo2.MsgReq2 msgReq2 = msg.getMsgReq2();

                    // check client
                    String peerId = msgReq2.getPeerId2();
                    Entry entry = mClientMap.getOrDefault(peerId, null);
                    if (entry == null) {
                        log("not push client is found");
                        return;
                    }

                    log("peer client got");

                    // convert to peer protocol
                    int innerSeq = genSeq();

                    final Demo1.DemoMsg convertedMsg;
                    {
                        Demo1.DemoMsg.Builder builder = Demo1.DemoMsg.newBuilder();
                        builder.setType1(Demo1.MsgType.MSG);
                        builder.setSeq1(innerSeq);
                        Demo1.MsgReq1.Builder msgReq1 = Demo1.MsgReq1.newBuilder();
                        msgReq1.setMsg1(msgReq2.getMsg2());
                        builder.setMsgReq1(msgReq1);

                        convertedMsg = builder.build();
                    }

                    // save route first
                    mRouteMap.put(innerSeq, new RouteRecord(innerSeq, msg.getSeq2(), mClientId));

                    // send
                    Channel peerChannel = entry.getChannel();

                    peerChannel.writeAndFlush(convertedMsg);

                    log("msg routed");

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
