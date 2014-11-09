package com.letsmidi.monsys.session;

/**
 * Created by zero on 8/11/14.
 */
public interface ISession<T> {
  T getKey();

  void close();

  boolean isTimeout();

  void timeout();
}
