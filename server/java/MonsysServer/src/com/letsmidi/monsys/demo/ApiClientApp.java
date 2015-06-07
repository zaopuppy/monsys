package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.util.BaseClientConnection;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;

/**
 * Created by zhaoyi on 15-6-7.
 */
public class ApiClientApp {

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) {

    }

    private static class ApiClientConnection extends BaseClientConnection<Push.PushMsg> {

        public ApiClientConnection(NioEventLoopGroup group) {
            super(group);
        }

        private SimpleChannelInboundHandler<Push.PushMsg> mHandler = new SimpleChannelInboundHandler<Push.PushMsg>() {
            @Override
            protected void channelRead0(ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
                log("received: " + msg.getType());

                onResponse(msg);
            }
        };

        public ChannelFuture login() {
            ChannelPromise promise = channel().newPromise();

            // TODO
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
                        Push.PushMsg.getDefaultInstance()));
                pipeline.addLast("handler", mHandler);
            }
        }

        @Override
        public Channel popChannel() {
            Channel channel = channel();
            if (channel == null) {
                return null;
            }

            ChannelPipeline pipeline = channel.pipeline();

            pipeline.remove("header-prepender");
            pipeline.remove("frame-decoder");
            pipeline.remove("encoder");
            pipeline.remove("decoder");
            pipeline.remove("handler");

            return channel;
        }

        @Override
        protected boolean saveRoute(Push.PushMsg msg, Callback<Push.PushMsg> callback) {
            getRouteMap().put(msg.getSequence(), new RouteItem<>(msg.getSequence(), callback));
            return true;
        }

        @Override
        protected RouteItem<Push.PushMsg> removeRoute(Push.PushMsg msg) {
            return getRouteMap().remove(msg.getSequence());
        }
    }
}
