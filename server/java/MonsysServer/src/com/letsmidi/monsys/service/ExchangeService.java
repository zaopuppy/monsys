package com.letsmidi.monsys.service;


import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import com.google.protobuf.InvalidProtocolBufferException;
import com.letsmidi.monsys.log.Logger;
import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;

// TODO: route record timeout
/**
 * everything is concurrent
 */
public class ExchangeService extends DefaultConsumer {

    private final static String TAG = "RouteService";

    private final static String QUEUE_NAME = "monsys://exchange/api";

    public final static ExchangeService INSTANCE = ExchangeService.create();

    public static interface Callback {
        void onWriteComplete();
        void onResponse(Exchange.ExchangeMsg rsp);
    }

    // data-structure
    public static class RouteItem {
        private final int sequence;
        //private final io.netty.channel.Channel channel;
        private final Callback callback;

        public RouteItem(
                int sequence, /*io.netty.channel.Channel channel,*/ Callback callback) {
            this.sequence = sequence;
            //this.channel = channel;
            this.callback = callback;
        }
    }

    private final Map<Integer, RouteItem> mRouteRecordMap = new HashMap<>();
    private final SequenceGenerator mSequenceGenerate = new SequenceGenerator(0, 0xFFFFFF);

    private ExchangeService(Channel channel) {
        super(channel);
    }

    //public static ExchangeService instance() {
    //    if (sInstance)
    //}

    public static ExchangeService create() {
        try {
            ConnectionFactory factory = new ConnectionFactory();
            factory.setHost("localhost");
            Connection connection = factory.newConnection();

            Channel channel = connection.createChannel();

            return new ExchangeService(channel);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public boolean start() {
        try {
            getChannel().queueDeclare(QUEUE_NAME, false, false, false, null);

            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    private void send(Exchange.ExchangeMsg msg, Callback callback) {
        try {
            if (!saveRoute(msg, callback)) {
                Logger.e(TAG, "failed to save route record");
                return;
            }
            getChannel().basicPublish("", QUEUE_NAME, null, msg.toByteArray());
            if (callback != null) {
                callback.onWriteComplete();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean saveRoute(Exchange.ExchangeMsg msg, Callback callback) {
        int sequence = msg.getSequence();
        if (mRouteRecordMap.containsKey(sequence)) {
            return false;
        }

        mRouteRecordMap.putIfAbsent(sequence, new RouteItem(msg.getSequence(), callback));
        return true;
    }

    @Override
    public void handleDelivery(String consumerTag, Envelope envelope,
                               AMQP.BasicProperties properties, byte[] body) throws IOException {
        Exchange.ExchangeMsg msg;
        try {
            msg = Exchange.ExchangeMsg.getDefaultInstance().getParserForType().parseFrom(body);
        } catch (InvalidProtocolBufferException e) {
            Logger.e(TAG, "Failed to decode delivery");
            e.printStackTrace();
            return;
        }


        RouteItem item = findRouteItem(msg);
        if (item == null) {
            Logger.w(TAG, "no route record was found");
            return;
        }

        if (item.callback != null) {
            item.callback.onResponse(msg);
        }
    }

    private RouteItem findRouteItem(Exchange.ExchangeMsg msg) {
        return mRouteRecordMap.getOrDefault(msg.getSequence(), null);
    }

    // TODO: abstract this as an interface
    public void requestForExchange(Callback callback) {
        Exchange.ExchangeMsg.Builder builder = MsgUtil.newExchangeMsgBuilder(Exchange.MsgType.REQUEST_EXCHANGE, nextSequence());
        Exchange.RequestExchange.Builder request = Exchange.RequestExchange.newBuilder();
        builder.setRequestExchange(request);
        send(builder.build(), callback);
    }

    private int nextSequence() {
        synchronized (mSequenceGenerate) {
            return mSequenceGenerate.next();
        }
    }
}

