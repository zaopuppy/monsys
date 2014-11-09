package com.letsmidi.monsys.http;

import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.handler.codec.http.*;
import io.netty.util.CharsetUtil;

import static io.netty.handler.codec.http.HttpHeaders.Names.CONTENT_TYPE;
import static io.netty.handler.codec.http.HttpVersion.HTTP_1_1;

/**
 * Created by zero on 8/23/14.
 */
public class HttpApiServerHandler extends SimpleChannelInboundHandler<HttpRequest> {
  @Override
  protected void channelRead0(ChannelHandlerContext ctx, HttpRequest msg) throws Exception {
    if (!msg.getDecoderResult().isSuccess()) {
      sendError(ctx, HttpResponseStatus.BAD_REQUEST);
      return;
    }

    if (msg.getMethod() != HttpMethod.POST) {
      sendError(ctx, HttpResponseStatus.METHOD_NOT_ALLOWED);
      return;
    }

    FullHttpResponse rsp = new DefaultFullHttpResponse(
        HttpVersion.HTTP_1_1, HttpResponseStatus.OK,
        Unpooled.copiedBuffer("POST works", CharsetUtil.UTF_8));

    ctx.writeAndFlush(rsp).addListener(ChannelFutureListener.CLOSE);
  }

  private void sendError(ChannelHandlerContext ctx, HttpResponseStatus status) {
    FullHttpResponse response = new DefaultFullHttpResponse(
        HTTP_1_1, status,
        Unpooled.copiedBuffer("Failure: " + status + "\r\n", CharsetUtil.UTF_8));

    response.headers().set(CONTENT_TYPE, "text/plain; charset=UTF-8");

    // Close the connection as soon as the error message is sent.
    ctx.writeAndFlush(response).addListener(ChannelFutureListener.CLOSE);
  }
}
