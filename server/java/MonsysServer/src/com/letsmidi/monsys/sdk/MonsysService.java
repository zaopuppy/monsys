package com.letsmidi.monsys.sdk;

import com.letsmidi.monsys.protocol.push.Push;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by zero on 8/24/14.
 */
public class MonsysService {

  public static class SessionManager {
    private Map<Integer, Session> mSessionMap = new HashMap<>(8);

    public Session find(Integer key) {
      return mSessionMap.getOrDefault(key, null);
    }

    public void add(Session session) {
      mSessionMap.put(session.getKey(), session);
    }

    public void remove(Integer key) {
      mSessionMap.remove(key);
    }
  }

  private static class Session {
    private final Integer key;
    private Callback callback;

    public Session(Integer key) {
      this.key = key;
    }

    public Integer getKey() { return key; }
  }

  public interface EventListener {
    void onDisconnected();
    void onConnected();
    void onException();
    void onNotify();
  }

  public interface Callback {
    void onComplete();
    void onException();
  }

  public void connect(Callback callback) {
    callback.onComplete();
  }

  public void login(Runnable runnable) {
    String msg = "msg";
    int sequence = 1;
    //send(msg);
  }

  private void run() {
    while (true) {
      //Push.PushMsg msg = receive();
      //switch (msg.getType()) {
      //  case HEARTBEAT: {
      //    break;
      //  }
      //  case USER_REGISTER_RSP:
      //  case CLIENT_LOGIN_RSP:
      //  case GET_FGW_LIST_RSP:
      //  case GET_DEV_LIST_RSP:
      //  case GET_DEV_INFO_RSP: {
      //    break;
      //  }
      //  default:
      //    break;
      //}
    }
  }
}
