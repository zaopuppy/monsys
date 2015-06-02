package com.letsmidi.monsys.exchange;


import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class ApiClientHandler extends SimpleChannelInboundHandler<Exchange.ExchangeMsg> {

    private static final SequenceGenerator sIdGenerator = new SequenceGenerator(0, 0xFFFFFF);

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) throws Exception {
        //switch (msg.getType()) {
        //    case REQUEST_EXCHANGE:
        //        handleRequestExchange(ctx, msg);
        //        break;
        //    default:
        //        ctx.close();
        //        break;
        //}
    }

    private void handleRequestExchange(ChannelHandlerContext ctx, Exchange.ExchangeMsg msg) {
        //if (!msg.hasRequestExchange()) {
        //    ctx.close();
        //    return;
        //}
        //
        //String id = generateId();
        //String peerId1 = generateId();
        //String peerId2 = generateId();
        //
        //ExchangeInfo exchangeInfo = new ExchangeInfo(id, peerId1, peerId2);
        //synchronized (InMemInfo.CLIENTS_MAP) {
        //    InMemInfo.CLIENTS_MAP.put(id, exchangeInfo);
        //}
        //
        //Exchange.ExchangeMsg.Builder builder = MsgUtil.newExchangeMsgBuilder(Exchange.MsgType.REQUEST_EXCHANGE_RSP, msg.getSequence());
        //Exchange.RequestExchangeRsp.Builder exchangeRsp = Exchange.RequestExchangeRsp.newBuilder();
        //exchangeRsp.setCode(0);
        //exchangeRsp.setId(id);
        //exchangeRsp.setPeerId1(peerId1);
        //exchangeRsp.setPeerId1(peerId2);
        //builder.setRequestExchangeRsp(exchangeRsp);
        //
        //ctx.writeAndFlush(builder.build());
    }

    private String generateId() {
        synchronized (sIdGenerator) {
            return "" + System.currentTimeMillis() + "-" + sIdGenerator.next();
        }
    }
}
