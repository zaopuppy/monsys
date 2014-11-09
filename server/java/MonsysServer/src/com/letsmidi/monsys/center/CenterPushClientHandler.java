package com.letsmidi.monsys.center;

import com.letsmidi.monsys.protocol.push.Push;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

/**
 * Created by zero on 9/15/14.
 */
public class CenterPushClientHandler extends SimpleChannelInboundHandler<Push.PushMsg> {
  @Override
  protected void channelRead0(ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
    //
  }
}
