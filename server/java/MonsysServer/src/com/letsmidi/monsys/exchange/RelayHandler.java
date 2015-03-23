package com.letsmidi.monsys.exchange;

import java.util.logging.Logger;


import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.ReferenceCountUtil;

public class RelayHandler extends SimpleChannelInboundHandler<ByteBuf> {
    private final Channel mRelayChannel;

    public RelayHandler(Channel ch) {
        mRelayChannel = ch;
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        ctx.writeAndFlush(Unpooled.EMPTY_BUFFER);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        //mLogger.info(String.format("Channel closed, local=(%s), remote=(%s)",
        //        ctx.channel().localAddress().toString(),
        //        ctx.channel().remoteAddress().toString()));
        if (mRelayChannel.isActive()) {
            mRelayChannel.flush();
            mRelayChannel.close();
            // SocksServerUtils.closeOnFlush(relayChannel);
        }
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        //mLogger.severe(cause.toString());
        //mLogger.severe("msg: " + cause.getMessage());
        ctx.close();
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, ByteBuf msg) throws Exception {
        if (mRelayChannel.isActive()) {
            mRelayChannel.writeAndFlush(msg);
        } else {
            ReferenceCountUtil.release(msg);
        }
    }

}