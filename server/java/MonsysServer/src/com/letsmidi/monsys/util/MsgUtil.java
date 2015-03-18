package com.letsmidi.monsys.util;

import com.letsmidi.monsys.protocol.center.Center;
import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.route.Route;

public class MsgUtil {
    public static final int VERSION = 1;

    //private static final SequenceGenerator mSeqenceGenerator = new SequenceGenerator(1, 0xFFFFFFFE);

    public static Exchange.ExchangeMsg.Builder newExchangeMsgBuilder(Exchange.MsgType type, int seq) {
        Exchange.ExchangeMsg.Builder builder = Exchange.ExchangeMsg.newBuilder();
        builder.setVersion(VERSION);
        builder.setType(type);
        builder.setSequence(seq);

        return builder;
    }

    public static CommServer.CommServerMsg.Builder newCommServerMsgBuilder(CommServer.MsgType type, int seq) {
        CommServer.CommServerMsg.Builder builder = CommServer.CommServerMsg.newBuilder();
        builder.setVersion(VERSION);
        builder.setType(type);
        //builder.setSequence(mSeqenceGenerator.next());
        builder.setSequence(seq);

        return builder;
    }

    public static Client.ClientMsg.Builder newClientMsgBuilder(Client.MsgType type, int seq) {
        Client.ClientMsg.Builder builder = Client.ClientMsg.newBuilder();
        builder.setVersion(VERSION);
        builder.setType(type);
        //builder.setSequence(mSeqenceGenerator.next());
        builder.setSequence(seq);

        return builder;
    }

    public static Center.CenterMsg.Builder newCenterMsgBuilder(Center.MsgType type, int seq) {
        Center.CenterMsg.Builder builder = Center.CenterMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        //builder.setSequence(mSeqenceGenerator.next());
        builder.setSequence(seq);

        return builder;
    }

    public static Push.PushMsg.Builder newPushMsgBuilder(Push.MsgType type, int seq) {
        Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        //builder.setSequence(mSeqenceGenerator.next());
        builder.setSequence(seq);

        return builder;
    }

    public static Route.RouteMsg.Builder newRouteMsgBuilder(Route.MsgType type, int seq) {
        Route.RouteMsg.Builder builder = Route.RouteMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        //builder.setSequence(mSeqenceGenerator.next());
        builder.setSequence(seq);

        return builder;
    }


}
