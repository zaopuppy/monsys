package com.letsmidi.monsys.push;

import com.letsmidi.monsys.Config;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.ReferenceCountUtil;

import java.util.logging.Logger;

public class FgwRelayHandler extends ChannelInboundHandlerAdapter {

  private final Channel relayChannel;

  private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

  public FgwRelayHandler(Channel relayChannel) {
    this.relayChannel = relayChannel;
  }

  @Override
  public void channelActive(ChannelHandlerContext ctx) {
    ctx.writeAndFlush(Unpooled.EMPTY_BUFFER);
  }

  @Override
  public void channelRead(ChannelHandlerContext ctx, Object msg) {
    if (relayChannel.isActive()) {
      relayChannel.writeAndFlush(msg);
    } else {
      ReferenceCountUtil.release(msg);
    }
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) {
    mLogger.info(String.format("Channel closed, local=(%s), remote=(%s)",
        ctx.channel().localAddress().toString(),
        ctx.channel().remoteAddress().toString()));
    if (relayChannel.isActive()) {
      relayChannel.flush();
      relayChannel.close();
      // SocksServerUtils.closeOnFlush(relayChannel);
    }
  }

  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
    cause.printStackTrace();
    mLogger.severe(cause.toString());
    mLogger.severe("msg: " + cause.getMessage());
    ctx.close();
    // XXX: should we call relayChannel.close() or not?
  }
}
