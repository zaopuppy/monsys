package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.demo1.Demo1;
import com.letsmidi.monsys.util.BaseClientConnection;
import com.letsmidi.monsys.util.NettyUtil;
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
 * Created by zero on 6/2/15.
 */
public class Demo1ClientApp {

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        NioEventLoopGroup worker = new NioEventLoopGroup(1);
        Demo1ClientConnection conn = new Demo1ClientConnection(worker);

        ChannelFuture f = conn.connect("127.0.0.1", 1983);

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

    private static class Demo1ClientConnection extends BaseClientConnection<Demo1.DemoMsg> {

        public Demo1ClientConnection(NioEventLoopGroup group) {
            super(group);
        }

        ChannelFuture login() {
            final ChannelPromise promise = channel().newPromise();

            Demo1.DemoMsg.Builder builder = Demo1.DemoMsg.newBuilder();
            builder.setType1(Demo1.MsgType.LOGIN);
            builder.setId1("push-client-id01");
            Demo1.LoginReq1.Builder loginReq1 = Demo1.LoginReq1.newBuilder();
            loginReq1.setId1("push-client-id01");

            builder.setLoginReq1(loginReq1);

            writeAndFlush(builder.build(), msg -> {
                if (msg == null || !msg.hasLoginRsp1()) {
                    promise.setFailure(new Throwable());
                    return;
                }

                Demo1.LoginRsp1 rsp1 = msg.getLoginRsp1();
                if (rsp1.getCode1() == 0) {
                    promise.setSuccess();
                } else {
                    promise.setFailure(new Throwable());
                }
            });

            return promise;
        }

        public void handle(String msg) {
            Demo1.DemoMsg.Builder builder = Demo1.DemoMsg.newBuilder();

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
                    Demo1.DemoMsg.getDefaultInstance()));
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
        protected boolean saveRoute(Demo1.DemoMsg msg, Callback<Demo1.DemoMsg> callback) {
            getRouteMap().put(0, new RouteItem<>(0, callback));
            return true;
        }

        @Override
        protected RouteItem<Demo1.DemoMsg> findRoute(Demo1.DemoMsg msg) {
            return getRouteMap().getOrDefault(0, null);
        }

        private final SimpleChannelInboundHandler<Demo1.DemoMsg> mHandler =
            new SimpleChannelInboundHandler<Demo1.DemoMsg>() {
                @Override
                protected void channelRead0(ChannelHandlerContext ctx, Demo1.DemoMsg msg) throws Exception {
                    //
                }
            };
    }
}

