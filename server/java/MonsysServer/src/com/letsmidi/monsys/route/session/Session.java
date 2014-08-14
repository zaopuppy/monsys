package com.letsmidi.monsys.route.session;

import io.netty.channel.ChannelHandlerContext;

import com.letsmidi.monsys.protocol.route.Route.RouteMsg;

public abstract class Session implements ISession {

  private final String mKey;
  private final SessionManager mManager;

  private long mTimeout = 60*1000;
  private long mLastTouchedTime;

  public Session(SessionManager manager, String key) {
    mManager = manager;
    mKey = key;
    // TODO: remove this! the less api invoking, the faster we gonna be!
    mLastTouchedTime = System.currentTimeMillis();
  }

  @Override
  public String getKey() {
    return mKey;
  }

  @Override
  public void close() {
    mManager.remove(this);
  }

  @Override
  public boolean isTimeout() {
    // TODO: DON'T get system time every time!
    long cur_time = System.currentTimeMillis();
    if (cur_time > mLastTouchedTime + mTimeout) {
      return true;
    }
    return false;
  }

  @Override
  public void timeout() {
    close();
  }
}
