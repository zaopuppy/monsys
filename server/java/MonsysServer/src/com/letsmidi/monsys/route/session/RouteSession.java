package com.letsmidi.monsys.route.session;

import com.letsmidi.monsys.session.Session;
import com.letsmidi.monsys.session.SessionManager;
import io.netty.channel.Channel;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * TODO: other client/fgw can't access anymore if this session has been established
 * TODO: once disconnected, this session should delete itself immediately
 */
public class RouteSession extends Session {

  public final String token;
  public int client_ip_v4;
  public int fgw_ip_v4;

  public AtomicBoolean client_connected = new AtomicBoolean(false);
  public AtomicBoolean fgw_connected = new AtomicBoolean(false);
  public Channel client_channel = null;
  public Channel fgw_channel = null;

  public RouteSession(SessionManager manager, String key) {
    super(manager, key);
    token = key;
  }

}
