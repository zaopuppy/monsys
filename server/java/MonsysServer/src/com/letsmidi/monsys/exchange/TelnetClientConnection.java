package com.letsmidi.monsys.exchange;

import com.letsmidi.monsys.util.BaseClientConnection;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.DelimiterBasedFrameDecoder;
import io.netty.handler.codec.Delimiters;
import io.netty.handler.codec.string.StringDecoder;
import io.netty.handler.codec.string.StringEncoder;

import java.nio.charset.Charset;


public class TelnetClientConnection extends BaseClientConnection<String> {

    public TelnetClientConnection(NioEventLoopGroup group, Channel channel) {
        super(group);

        setChannel(channel);
    }

    @Override
    protected void setChannel(Channel channel) {
        super.setChannel(channel);

        if (channel == null) {
            return;
        }

        ChannelPipeline pipeline = channel.pipeline();

        pipeline.addLast("frame-decoder", new DelimiterBasedFrameDecoder(8192, Delimiters.lineDelimiter()));
        pipeline.addLast("decoder", new StringDecoder(Charset.forName("utf-8")));
        pipeline.addLast("encoder", new StringEncoder(Charset.forName("utf-8")));
        pipeline.addLast(mHandler);
    }

    @Override
    public Channel popChannel() {
        Channel channel = channel();
        if (channel == null) {
            return null;
        }

        channel.pipeline().remove("frame-decoder");
        channel.pipeline().remove("decoder");
        channel.pipeline().remove("encoder");

        return channel;
    }

    @Override
    protected boolean saveRoute(String msg, Callback<String> callback) {
        return false;
    }

    @Override
    protected RouteItem<String> removeRoute(String msg) {
        return null;
    }

    private static void log(String msg) {
        System.out.println(msg);
    }

    private SimpleChannelInboundHandler<String> mHandler = new SimpleChannelInboundHandler<String>() {

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
        public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
            super.exceptionCaught(ctx, cause);
            log("exceptionCaught");
            cause.printStackTrace();
        }

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, String msg) throws Exception {
            log("received: " + msg);
        }
    };
}

