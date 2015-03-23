package com.letsmidi.monsys.exchange;

import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.util.HashedWheelTimer;

import java.util.HashMap;
import java.util.logging.Logger;


public class ClientHandler extends SimpleChannelInboundHandler<Exchange.ExchangeMsg> {
    private final Logger mLogger = Logger.getLogger(ExchangeConfig.LoggerName);

    private static final HashMap<String, ChannelPair> PAIR_MAP = new HashMap<>();

    private static class ChannelPair {
        public String id = null;
        public Channel channel1 = null;
        public Channel channel2 = null;
    }

    public ClientHandler(HashedWheelTimer timer) {
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) throws Exception {
        mLogger.info("channelRead0");
        switch (msg.getType()) {
            case CONNECT:
                handleConnect(ctx, msg);
                break;
            default:
                break;
        }
    }

    private void handleConnect(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) {
        mLogger.info("handleConnect");

        if (!msg.hasConnect()) {
            ctx.close();
            return;
        }

        Exchange.Connect connect = msg.getConnect();
        String id = connect.getId();
        int type = connect.getType();

        mLogger.info("id=" + id + ", type=" + type);

        synchronized (PAIR_MAP) {
            ChannelPair pair = PAIR_MAP.getOrDefault(id, null);
            if (pair == null) {
                pair = new ChannelPair();
                pair.id = id;
                PAIR_MAP.put(id, pair);
            }

            //
            if (type == 1) {
                if (pair.channel1 != null) {
                    ctx.close();
                    return;
                }

                pair.channel1 = ctx.channel();
            } else if (type == 2) {
                if (pair.channel2 != null) {
                    ctx.close();
                    return;
                }

                pair.channel2 = ctx.channel();
            } else {
                ctx.close();
                return;
            }

            if (pair.channel1 != null && pair.channel2 != null) {
                Exchange.ExchangeMsg.Builder builder = MsgUtil.newExchangeMsgBuilder(Exchange.MsgType.CONNECT_RSP, 0);
                Exchange.ConnectRsp.Builder connect_rsp_builder = Exchange.ConnectRsp.newBuilder();
                connect_rsp_builder.setCode(0);
                builder.setConnectRsp(connect_rsp_builder);

                pair.channel1.writeAndFlush(builder.build());
                pair.channel2.writeAndFlush(builder.build());

                //
                pair.channel1.pipeline().remove(ProtobufVarint32LengthFieldPrepender.class);
                pair.channel1.pipeline().remove(ProtobufVarint32FrameDecoder.class);
                pair.channel1.pipeline().remove(ProtobufEncoder.class);
                pair.channel1.pipeline().remove(ProtobufDecoder.class);
                pair.channel1.pipeline().remove(ClientHandler.class);
                pair.channel1.pipeline().addLast(new RelayHandler(pair.channel2));
                //
                pair.channel2.pipeline().remove(ProtobufVarint32LengthFieldPrepender.class);
                pair.channel2.pipeline().remove(ProtobufVarint32FrameDecoder.class);
                pair.channel2.pipeline().remove(ProtobufEncoder.class);
                pair.channel2.pipeline().remove(ProtobufDecoder.class);
                pair.channel2.pipeline().remove(ClientHandler.class);
                pair.channel2.pipeline().addLast(new RelayHandler(pair.channel1));
            }
        }
    }
}
