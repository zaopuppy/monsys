package com.letsmidi.monsys.session;

public abstract class Session<T> implements ISession<T> {

  private final T mKey;
  private final SessionManager mManager;

  private long mTimeout = 60*1000;
  private long mLastTouchedTime;

  public Session(SessionManager manager, T key) {
    mManager = manager;
    mKey = key;
    // TODO: remove this! the less api invoking, the faster we gonna be!
    mLastTouchedTime = System.currentTimeMillis();
  }

  @Override
  public T getKey() {
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
