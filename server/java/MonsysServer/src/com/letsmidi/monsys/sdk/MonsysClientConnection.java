package com.letsmidi.monsys.sdk;

import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.util.BaseClientConnection;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;


public class MonsysClientConnection extends BaseClientConnection {

    private final ChannelHandler mHandler = new SimpleChannelInboundHandler<Client.ClientMsg>() {
        @Override
        protected void channelRead0(ChannelHandlerContext ctx, Client.ClientMsg msg) throws Exception {
            //
        }
    };

    public MonsysClientConnection(NioEventLoopGroup group) {
        super(group);
    }

    public ChannelFuture login() {
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.LOGIN, 0);

        return channel().write(builder.build());
    }

    @Override
    public void setChannel(Channel channel) {
        super.setChannel(channel);

        if (channel == null) {
            return;
        }

        ChannelPipeline pipeline = channel.pipeline();
        pipeline.addLast("frame-prepender", new ProtobufVarint32LengthFieldPrepender());
        pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
        pipeline.addLast("encoder", new ProtobufEncoder());
        pipeline.addLast("decoder", new ProtobufDecoder(Client.ClientMsg.getDefaultInstance()));
        pipeline.addLast("handler", mHandler);
    }

    @Override
    public Channel popChannel() {
        return null;
    }

    @Override
    protected boolean saveRoute(Object msg, Callback callback) {
        return false;
    }

    @Override
    protected RouteItem removeRoute(Object msg) {
        return null;
    }
}
