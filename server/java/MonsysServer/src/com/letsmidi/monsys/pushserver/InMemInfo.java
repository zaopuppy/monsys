package com.letsmidi.monsys.pushserver;

import com.letsmidi.monsys.pushserver.session.PushSession;
import com.letsmidi.monsys.session.SessionManager;
import io.netty.util.Timer;

/**
 * Created by z00302892 on 2015/2/16.
 */
public class InMemInfo {
    public static final InMemInfo INSTANCE = new InMemInfo();

    // disable default constructor
    private InMemInfo() {
    }

    public void init(Timer timer) {
        internalSessionMap = new SessionManager<>(timer);
    }

    private SessionManager<Integer, PushSession> internalSessionMap = null;

    public SessionManager<Integer, PushSession> getInternalSessionManager() {
        return internalSessionMap;
    }
}
