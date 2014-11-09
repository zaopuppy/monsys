package com.letsmidi.monsys.push.session;

import com.letsmidi.monsys.session.Session;
import com.letsmidi.monsys.session.SessionManager;

/**
 * Created by zero on 8/27/14.
 */
public class PushSession extends Session<Integer> {
  public PushSession(SessionManager manager, Integer key) {
    super(manager, key);
  }
}
