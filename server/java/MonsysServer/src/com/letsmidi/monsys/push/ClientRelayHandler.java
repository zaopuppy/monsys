package com.letsmidi.monsys.push;

import com.letsmidi.monsys.protocol.push.Push.*;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.ReferenceCountUtil;

import java.util.logging.Logger;

public class ClientRelayHandler extends SimpleChannelInboundHandler<PushMsg> {
  private final Channel mRelayChannel;
  private String mFgwList;
  private final Logger mLogger = Logger.getLogger(PushServer.LOGGER_NAME);

  public ClientRelayHandler(Channel ch) {
    mRelayChannel = ch;
  }

  @Override
  public void channelActive(ChannelHandlerContext ctx) throws Exception {
    ctx.writeAndFlush(Unpooled.EMPTY_BUFFER);
  }

  @Override
  public void channelInactive(ChannelHandlerContext ctx) throws Exception {
    mLogger.info("Channel closed");
    if (mRelayChannel.isActive()) {
      mRelayChannel.flush();
      mRelayChannel.close();
      // SocksServerUtils.closeOnFlush(relayChannel);
    }
  }

  @Override
  public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
    cause.printStackTrace();
    ctx.close();
  }

  @Override
  protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    mLogger.info("received: " + msg.getType());
    if (msg.getType() == MsgType.GET_FGW_LIST) {
      processGetFgwList(ctx, msg);
      return;
    } else if (msg.getType() == MsgType.CONNECT) {
      processConnect(ctx, msg);
      return;
    }

    if (mRelayChannel.isActive()) {
      mRelayChannel.writeAndFlush(msg);
    } else {
      ReferenceCountUtil.release(msg);
    }
  }

  private void processConnect(ChannelHandlerContext ctx, PushMsg msg) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.CONNECT_RSP);

    ConnectRsp.Builder rsp = ConnectRsp.newBuilder();
    rsp.setCode(0);

    builder.setConnectRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  private void processGetFgwList(ChannelHandlerContext ctx, PushMsg msg) {
    mLogger.info("processGetFgwList");
    if (!msg.hasGetFgwList()) {
      return;
    }

    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.GET_FGW_LIST_RSP);

    GetFgwListRsp.Builder rsp = GetFgwListRsp.newBuilder();
    rsp.setCode(0);
    FGatewayInfo.Builder info = FGatewayInfo.newBuilder();
    if (this.mFgwList != null) {
      String[] fgws = this.mFgwList.split("\\|");
      for (String fgw: fgws) {
        info.setId(fgw);
        info.setName(fgw);
        info.setDesc("nothing about this gateway");
        rsp.addFgwInfos(info.build());
      }
    }

    builder.setGetFgwListRsp(rsp);

    ctx.writeAndFlush(builder.build());
  }

  public void setFgwList(String fgw_list) {
    this.mFgwList = fgw_list;
  }
}