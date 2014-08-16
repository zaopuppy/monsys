package com.letsmidi.monsys.center;

import com.letsmidi.monsys.protocol.center.Center;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

/**
 * Created by zero on 8/16/14.
 */
public class CenterServerHandler extends SimpleChannelInboundHandler<Center.CenterMsg> {
  @Override
  protected void channelRead0(ChannelHandlerContext ctx, Center.CenterMsg msg) throws Exception {
    //
  }
}
