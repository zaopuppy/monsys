package com.letsmidi.monsys.session;

import com.letsmidi.monsys.log.MyLogger;
import io.netty.util.Timeout;
import io.netty.util.Timer;
import io.netty.util.TimerTask;

import java.util.Iterator;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;

public class SessionManager<T> {
  private final ConcurrentHashMap<T, Session<T>> mSessionMap = new ConcurrentHashMap<>();
  private final Timer mTimer;

  private TimerTask mTimerTask = new TimerTask() {
    @Override
    public void run(Timeout timeout) throws Exception {
      checkTimeout();
    }
  };

  public SessionManager(Timer timer) {
    mTimer = timer;
    mTimer.newTimeout(mTimerTask, 1, TimeUnit.SECONDS);
  }

  private void checkTimeout() {
    //Logger.d("session map size: " + mSessionMap.size());
    Iterator<ConcurrentHashMap.Entry<T, Session<T>>> iter;
    for (iter = mSessionMap.entrySet().iterator(); iter.hasNext(); /* do nothing */) {
      ConcurrentHashMap.Entry<T, Session<T>> entry = iter.next();
      Session session = entry.getValue();
      if (session.isTimeout()) {
        MyLogger.d("timeout: " + entry.getKey());
        session.timeout();
      }
    }
    mTimer.newTimeout(mTimerTask, 1, TimeUnit.SECONDS);
  }

  public void add(Session<T> session) {
    mSessionMap.put(session.getKey(), session);
  }

  public Session find(T key) {
    return mSessionMap.get(key);
  }

  public void remove(Session<T> session) {
    mSessionMap.remove(session.getKey());
  }

}