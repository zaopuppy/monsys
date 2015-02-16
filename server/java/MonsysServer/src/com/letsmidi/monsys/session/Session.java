package com.letsmidi.monsys.session;

/**
 * Created by zero on 8/11/14.
 */
public interface Session<T> {

    T getKey();

    void close();

    void checkTimeout(long current);

    //boolean isTimeout();

    //void timeout();
}
