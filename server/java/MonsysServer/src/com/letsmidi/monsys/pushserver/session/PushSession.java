package com.letsmidi.monsys.pushserver.session;

import com.letsmidi.monsys.session.BaseSession;
import com.letsmidi.monsys.session.SessionManager;

/**
 * Created by zero on 8/27/14.
 */
public class PushSession extends BaseSession<Integer> {
    public int external_sequence = -1;

    public PushSession(SessionManager manager, Integer key) {
        super(manager, key);
    }
}
