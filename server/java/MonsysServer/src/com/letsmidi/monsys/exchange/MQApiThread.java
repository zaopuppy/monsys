package com.letsmidi.monsys.exchange;


import java.io.IOException;

import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.protocol.exchange.Exchange;
import com.letsmidi.monsys.util.SequenceGenerator;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.QueueingConsumer;

public class MQApiThread extends Thread {

    private final static String QUEUE_NAME = "monsys://exchange/api";
    private final SequenceGenerator mIdGenerator = new SequenceGenerator(0, 0xFFFFFF);

    private static void log(String msg) {
        System.out.println(msg);
    }

    @Override
    public void run() {
        try {
            // MQ
            ConnectionFactory connectionFactory = new ConnectionFactory();
            connectionFactory.setHost("localhost");
            Connection connection = connectionFactory.newConnection();
            Channel channel = null;
            channel = connection.createChannel();
            channel.queueDeclare(QUEUE_NAME, false, false, false, null);

            QueueingConsumer consumer = new QueueingConsumer(channel);
            channel.basicConsume(QUEUE_NAME, true, consumer);

            log("MQThread is ready");

            for (QueueingConsumer.Delivery delivery = consumer.nextDelivery();
                    delivery != null; delivery = consumer.nextDelivery()) {
                // String message = new String(delivery.getBody());
                Exchange.ExchangeMsg msg = Exchange.ExchangeMsg.getDefaultInstance().getParserForType().parseFrom(delivery.getBody());
                log("received: " + msg.getType());
                handleMsg(msg);
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void handleMsg(Exchange.ExchangeMsg msg) {
        //
    }

    private String generateId() {
        synchronized (mIdGenerator) {
            return "" + System.currentTimeMillis() + "-" + mIdGenerator.next();
        }
    }
}
