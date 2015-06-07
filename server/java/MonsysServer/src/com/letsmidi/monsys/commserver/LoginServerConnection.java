package com.letsmidi.monsys.commserver;

import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.util.BaseClientConnection;
import io.netty.channel.Channel;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;

/**
 * Created by joey on 3/27/15.
 */
public class LoginServerConnection extends BaseClientConnection {

    public LoginServerConnection(NioEventLoopGroup group) {
        super(group);
    }

    @Override
    public void setChannel(Channel channel) {
        super.setChannel(channel);

        ChannelPipeline pipeline = channel.pipeline();
        pipeline.addLast("frame-prepender", new ProtobufVarint32LengthFieldPrepender());
        pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
        pipeline.addLast("encoder", new ProtobufEncoder());
        pipeline.addLast("decoder", new ProtobufDecoder(CommServer.CommServerMsg.getDefaultInstance()));
        pipeline.addLast("handler", new LoginServerHandler());
    }

    @Override
    public Channel popChannel() {
        Channel channel = channel();
        if (channel == null) {
            return null;
        }

        ChannelPipeline pipeline = channel.pipeline();

        pipeline.remove("frame-prepender");
        pipeline.remove("frame-decoder");
        pipeline.remove("encoder");
        pipeline.remove("decoder");
        pipeline.remove("handler");

        return channel;
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
