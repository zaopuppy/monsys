package com.letsmidi.monsys.session;

public class BaseSession<T> implements Session<T> {

    private final T mKey;
    private final SessionManager mManager;

    private long mTimeout = 60 * 1000;
    private long mLastTouchedTime;

    public BaseSession(SessionManager manager, T key) {
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
    public void checkTimeout(long current) {
        if (current > mLastTouchedTime + mTimeout) {
            close();
        }
    }

    //@Override
    //public boolean isTimeout() {
    //     // FIXME: DON'T get system time every time!
    //    long cur_time = System.currentTimeMillis();
    //    return cur_time > mLastTouchedTime + mTimeout;
    //}
    //
    //@Override
    //public void timeout() {
    //    close();
    //}
}
