package com.letsmidi.monsys.service;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;

public interface RouteService<T> {

    boolean start();

    void send(T msg);

    boolean saveRoute(T msg);
}
