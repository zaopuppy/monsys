package com.letsmidi.monsys.exchange;

import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;


public class ExchangeClientConnection extends BaseClientConnection<Exchange.ExchangeMsg> {

    private ChannelPromise mResultPromise = null;

    private final SimpleChannelInboundHandler<Exchange.ExchangeMsg> mHandler =
            new SimpleChannelInboundHandler<Exchange.ExchangeMsg>() {
                @Override
                public void channelActive(ChannelHandlerContext ctx) throws Exception {
                    super.channelActive(ctx);
                }

                @Override
                public void channelInactive(ChannelHandlerContext ctx) throws Exception {
                    super.channelInactive(ctx);
                }

                @Override
                protected void channelRead0(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) throws Exception {
                    log("channelRead0");
                    if (msg.getType() != Exchange.MsgType.CONNECT_RSP || !msg.hasConnectRsp()) {
                        log("bad response");
                        mResultPromise.setFailure(null);
                        return;
                    }
                    Exchange.ConnectRsp connect_rsp = msg.getConnectRsp();
                    if (connect_rsp.getCode() == 0) {
                        mResultPromise.setSuccess();
                    } else {
                        mResultPromise.setFailure(null);
                    }
                }
            };


    public ExchangeClientConnection(NioEventLoopGroup group) {
        super(group);
    }


    private static void log(String msg) {
        System.out.println(msg);
    }

    public ChannelFuture auth(String id, String peerId) {
        if (!channel().isActive()) {
            log("channel is not active");
            return null;
        }

        if (mResultPromise == null) {
            mResultPromise = channel().newPromise();
        }

        Exchange.ExchangeMsg.Builder builder = MsgUtil.newExchangeMsgBuilder(Exchange.MsgType.CONNECT, 0);

        Exchange.Connect.Builder connect = Exchange.Connect.newBuilder();
        connect.setId(id);
        connect.setPeerId(peerId);
        builder.setConnect(connect);

        channel().writeAndFlush(builder.build(), channel().newPromise().addListener(
                (ChannelFuture future) -> {
                    if (!future.isSuccess()) {
                        mResultPromise.setFailure(future.cause());
                    }
                }
        ));

        return mResultPromise;
    }

    @Override
    protected void setChannel(Channel channel) {
        super.setChannel(channel);
        if (channel != null) {
            ChannelPipeline pipeline = channel.pipeline();
            pipeline.addLast("header-prepender", new ProtobufVarint32LengthFieldPrepender());
            pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
            pipeline.addLast("encoder", new ProtobufEncoder());
            pipeline.addLast("decoder", new ProtobufDecoder(Exchange.ExchangeMsg.getDefaultInstance()));
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
}
