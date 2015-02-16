package com.letsmidi.monsys.session;

import java.util.Iterator;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;

import com.letsmidi.monsys.log.MyLogger;
import io.netty.util.Timer;
import io.netty.util.TimerTask;

public class SessionManager<KEY_T, SESSION_T extends Session<KEY_T>> {
    private final ConcurrentHashMap<KEY_T, SESSION_T> mSessionMap = new ConcurrentHashMap<>();
    private final Timer mTimer;

    private TimerTask mTimerTask = timeout -> checkTimeout();

    public SessionManager(Timer timer) {
        mTimer = timer;
        mTimer.newTimeout(mTimerTask, 1, TimeUnit.SECONDS);
    }

    private void checkTimeout() {
        long current = System.currentTimeMillis();
        Iterator<ConcurrentHashMap.Entry<KEY_T, SESSION_T>> iter;
        for (iter = mSessionMap.entrySet().iterator(); iter.hasNext(); /* do nothing */) {
            ConcurrentHashMap.Entry<KEY_T, SESSION_T> entry = iter.next();
            SESSION_T session = entry.getValue();
            session.checkTimeout(current);
        }
        mTimer.newTimeout(mTimerTask, 1, TimeUnit.SECONDS);
    }

    public void add(SESSION_T session) {
        mSessionMap.put(session.getKey(), session);
    }

    public SESSION_T find(KEY_T key) {
        return mSessionMap.get(key);
    }

    public void remove(SESSION_T session) {
        mSessionMap.remove(session.getKey());
    }

}
