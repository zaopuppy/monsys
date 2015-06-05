package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.demo2.Demo2;
import com.letsmidi.monsys.util.BaseClientConnection;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * as an Admin
 *
 * Created by zero on 6/2/15.
 */
public class Demo2ClientApp {

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        NioEventLoopGroup worker = new NioEventLoopGroup(1);
        Demo2ClientConnection conn = new Demo2ClientConnection(worker);

        ChannelFuture f = conn.connect("127.0.0.1", 1984);

        // wait util we connected
        f.sync();

        if (!f.isSuccess()) {
            log("failed to connect server");
            return;
        }

        f = conn.login();
        f.sync();
        if (!f.isSuccess()) {
            log("failed to login");
            return;
        }

        log("login success");

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        for (String line = reader.readLine(); line != null; line = reader.readLine()) {
            conn.handle(line);
        }
    }

    private static class Demo2ClientConnection extends BaseClientConnection<Demo2.DemoMsg> {

        public Demo2ClientConnection(NioEventLoopGroup group) {
            super(group);
        }

        ChannelFuture login() {
            final ChannelPromise promise = channel().newPromise();

            Demo2.DemoMsg.Builder builder = Demo2.DemoMsg.newBuilder();
            builder.setType2(Demo2.MsgType.LOGIN);
            Demo2.LoginReq2.Builder loginReq2 = Demo2.LoginReq2.newBuilder();
            loginReq2.setId2("admin-id01");

            builder.setLoginReq2(loginReq2);

            writeAndFlush(builder.build(), msg -> {
                if (msg == null || !msg.hasLoginRsp2()) {
                    promise.setFailure(new Throwable());
                    return;
                }

                Demo2.LoginRsp2 rsp1 = msg.getLoginRsp2();
                if (rsp1.getCode2() == 0) {
                    promise.setSuccess();
                } else {
                    promise.setFailure(new Throwable());
                }
            });

            return promise;
        }

        public void handle(String msg) {
            Demo2.DemoMsg.Builder builder = Demo2.DemoMsg.newBuilder();
            builder.setType2(Demo2.MsgType.MSG);

            Demo2.MsgReq2.Builder msgReq2 = Demo2.MsgReq2.newBuilder();
            msgReq2.setMsg2(msg);
            msgReq2.setPeerId2("push-client-id01");

            builder.setMsgReq2(msgReq2);

            channel().writeAndFlush(builder.build());

            log("msg sent");
        }

        @Override
        protected void setChannel(Channel channel) {
            super.setChannel(channel);
            if (channel != null) {
                ChannelPipeline pipeline = channel.pipeline();
                pipeline.addLast("header-prepender", new ProtobufVarint32LengthFieldPrepender());
                pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
                pipeline.addLast("encoder", new ProtobufEncoder());
                pipeline.addLast("decoder", new ProtobufDecoder(
                    Demo2.DemoMsg.getDefaultInstance()));
                pipeline.addLast("handler", mHandler);
            }
        }

        @Override
        public Channel popChannel() {
            Channel channel = channel();
            if (channel == null) {
                return null;
            }

            channel.pipeline().remove("header-prepender");
            channel.pipeline().remove("frame-decoder");
            channel.pipeline().remove("encoder");
            channel.pipeline().remove("decoder");
            channel.pipeline().remove("handler");

            return channel;
        }

        @Override
        protected boolean saveRoute(Demo2.DemoMsg msg, Callback<Demo2.DemoMsg> callback) {
            getRouteMap().put(0, new RouteItem<>(0, callback));
            return true;
        }

        @Override
        protected RouteItem<Demo2.DemoMsg> findRoute(Demo2.DemoMsg msg) {
            return getRouteMap().getOrDefault(0, null);
        }

        private final SimpleChannelInboundHandler<Demo2.DemoMsg> mHandler =
            new SimpleChannelInboundHandler<Demo2.DemoMsg>() {
                @Override
                protected void channelRead0(ChannelHandlerContext ctx, Demo2.DemoMsg msg) throws Exception {
                    switch (msg.getType2()) {
                        case MSG:
                            log("received: " + msg.getMsgReq2().getMsg2());
                            break;
                        default:
                            onResponse(msg);
                            break;
                    }
                }
            };
    }
}

