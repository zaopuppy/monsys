package com.letsmidi.monsys.util;

import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.protocol.center.Center;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.route.Route;

/**
 * Created by zero on 8/24/14.
 */
public class MsgUtil {
    public static final int VERSION = 1;

    private static final SequenceGenerator mSeqenceGenerator = new SequenceGenerator(1, 0xFFFFFFFE);

    public static Client.ClientMsg.Builder newClientMsgBuilder(Client.MsgType type) {
        Client.ClientMsg.Builder builder = Client.ClientMsg.newBuilder();
        builder.setVersion(VERSION);
        builder.setType(type);
        builder.setSequence(mSeqenceGenerator.next());

        return builder;
    }

    public static Center.CenterMsg.Builder newCenterMsgBuilder(Center.MsgType type) {
        Center.CenterMsg.Builder builder = Center.CenterMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        builder.setSequence(mSeqenceGenerator.next());

        return builder;
    }

    public static Push.PushMsg.Builder newPushMsgBuilder(Push.MsgType type) {
        Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        // TODO: should be various every time
        builder.setSequence(mSeqenceGenerator.next());

        return builder;
    }

    public static Route.RouteMsg.Builder newRouteMsgBuilder(Route.MsgType type) {
        Route.RouteMsg.Builder builder = Route.RouteMsg.newBuilder();
        builder.setVersion(1);
        builder.setType(type);
        builder.setSequence(mSeqenceGenerator.next());

        return builder;
    }


}
