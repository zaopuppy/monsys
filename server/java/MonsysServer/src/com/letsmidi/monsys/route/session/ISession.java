package com.letsmidi.monsys.route.session;

/**
 * Created by zero on 8/11/14.
 */
public interface ISession {
  String getKey();

  //boolean isComplete();

  void close();

  boolean isTimeout();

  void timeout();
}
