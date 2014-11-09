package com.letsmidi.monsys.center;

import com.letsmidi.monsys.protocol.route.Route;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

/**
 * Created by zero on 9/15/14.
 */
public class CenterRouterClientHandler extends SimpleChannelInboundHandler<Route.RouteMsg> {
  //private enum State {
  //  STATE_WAIT_FOR_LOGIN,
  //  STATE_LOGGED_IN,
  //}
  //
  //private State mState = State.STATE_WAIT_FOR_LOGIN;

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, Route.RouteMsg msg) throws Exception {
    //switch (mState) {
    //  case STATE_WAIT_FOR_LOGIN:
    //    onRead_WaitForLogin(ctx, msg);
    //    break;
    //  case STATE_LOGGED_IN:
    //    break;
    //  default:
    //    break;
    //}
  }
}
