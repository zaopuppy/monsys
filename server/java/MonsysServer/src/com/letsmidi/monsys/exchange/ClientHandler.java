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

import java.util.logging.Logger;


/**
 * TODO: 等待first request超时
 * TODO: 每个client等待peer超时
 * TODO: 每个记录有一个超时
 */
public class ClientHandler extends SimpleChannelInboundHandler<Exchange.ExchangeMsg> {

    private final Logger mLogger = Logger.getLogger(ExchangeConfig.LoggerName);

    private String mId = null;
    private String mPeerId = null;

    // private static final HashMap<String, ChannelPair> PAIR_MAP = new HashMap<>();

    private enum State {
        WAIT_FOR_CONNECT,
        WAIT_FOR_PEER,
        CONNECTED,
    }

    private State mState = State.WAIT_FOR_CONNECT;

    public ClientHandler(HashedWheelTimer timer) {
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) throws Exception {
        mLogger.info("channelRead0");

        switch (mState) {
            case WAIT_FOR_CONNECT: {
                switch (msg.getType()) {
                    case CONNECT:
                        handleConnect(ctx, msg);
                        break;
                    default:
                        break;
                }
                break;
            }
            default:
                mLogger.info("bad state, ignore");
                break;
        }
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        mLogger.info("channelInactive");
        if (mState == State.WAIT_FOR_PEER) {
            assert(mId != null);
            assert(mPeerId != null);
            synchronized (InMemInfo.CLIENTS_MAP) {
                ExchangeInfo exchangeInfo = InMemInfo.CLIENTS_MAP.getOrDefault(mId, null);
                if (exchangeInfo == null) {
                    return;
                }

                if (mPeerId.equals(exchangeInfo.getPeerId1())) {
                    exchangeInfo.setPeerChannel1(null);
                } else if (mPeerId.equals(exchangeInfo.getPeerId2())) {
                    exchangeInfo.setPeerChannel2(null);
                } else {
                    // warning
                }
            }
        }
    }

    private void handleConnect(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) {
        mLogger.info("handleConnect");

        if (!msg.hasConnect()) {
            ctx.close();
            return;
        }

        Exchange.Connect connect = msg.getConnect();
        mId = connect.getId();
        //mPeerId = connect.getPeerId();

        mLogger.info("id=" + mId + ", peerId=" + mPeerId);

        if (mId == null || mId.isEmpty() || mPeerId == null || mPeerId.isEmpty()) {
            ctx.close();
            return;
        }

        synchronized (InMemInfo.CLIENTS_MAP) {

            // find or create a pair
            ExchangeInfo exchangeInfo = InMemInfo.CLIENTS_MAP.getOrDefault(mId, null);
            if (exchangeInfo == null) {
                mLogger.warning("unregistered client id");
                ctx.close();
                return;
            }

            //
            if (mPeerId.equals(exchangeInfo.getPeerId1())) {
                if (exchangeInfo.getPeerChannel1() != null) {
                    ctx.close();
                    return;
                }

                exchangeInfo.setPeerChannel1(ctx.channel());
                ctx.channel().closeFuture().addListener(future -> exchangeInfo.setPeerChannel1(null));
            } else if (mPeerId.equals(exchangeInfo.getPeerId2())) {
                if (exchangeInfo.getPeerChannel2() != null) {
                    ctx.close();
                    return;
                }

                exchangeInfo.setPeerChannel2(ctx.channel());
                ctx.channel().closeFuture().addListener(future -> exchangeInfo.setPeerChannel2(null));
            } else {
                mLogger.severe("bad peer id");
                ctx.close();
                return;
            }

            mState = State.WAIT_FOR_PEER;

            Channel channel1 = exchangeInfo.getPeerChannel1();
            Channel channel2 = exchangeInfo.getPeerChannel2();

            if (channel1 != null && channel2 != null) {
                // response
                Exchange.ExchangeMsg.Builder builder = MsgUtil.newExchangeMsgBuilder(Exchange.MsgType.CONNECT_RSP, 0);
                Exchange.ConnectRsp.Builder connect_rsp_builder = Exchange.ConnectRsp.newBuilder();
                connect_rsp_builder.setCode(0);
                builder.setConnectRsp(connect_rsp_builder);

                // TODO: encode response first, then send it at the last.
                // byte[] encodedRsp = builder.build().toByteArray()
                channel1.writeAndFlush(builder.build());
                channel2.writeAndFlush(builder.build());

                //
                channel1.pipeline().remove(ProtobufVarint32LengthFieldPrepender.class);
                channel1.pipeline().remove(ProtobufVarint32FrameDecoder.class);
                channel1.pipeline().remove(ProtobufEncoder.class);
                channel1.pipeline().remove(ProtobufDecoder.class);
                channel1.pipeline().remove(ClientHandler.class);
                channel1.pipeline().addLast(new RelayHandler(channel2));
                //
                channel2.pipeline().remove(ProtobufVarint32LengthFieldPrepender.class);
                channel2.pipeline().remove(ProtobufVarint32FrameDecoder.class);
                channel2.pipeline().remove(ProtobufEncoder.class);
                channel2.pipeline().remove(ProtobufDecoder.class);
                channel2.pipeline().remove(ClientHandler.class);
                channel2.pipeline().addLast(new RelayHandler(channel1));

                mState = State.CONNECTED;
            }
        }
    }
}
