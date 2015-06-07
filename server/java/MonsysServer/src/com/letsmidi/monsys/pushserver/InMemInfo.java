package com.letsmidi.monsys.pushserver;

import com.letsmidi.monsys.pushserver.session.PushSession;
import com.letsmidi.monsys.session.SessionManager;
import io.netty.util.Timer;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

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

    // Router Map
    private final ConcurrentHashMap<Integer, RouteRecord> routeMap = new ConcurrentHashMap<>(10000);

    public ConcurrentHashMap<Integer, RouteRecord> getRouteMap() {
        return routeMap;
    }

    // Api Client Map
    private final ConcurrentHashMap<String, ApiClient> apiClientMap = new ConcurrentHashMap<>();

    public ConcurrentHashMap<String, ApiClient> getApiClientMap() {
        return apiClientMap;
    }

    // Push Client map
    private final ConcurrentHashMap<String, PushClient> pushClientMap = new ConcurrentHashMap<>(10000);

    public ConcurrentHashMap<String, PushClient> getPushClientMap() {
        return pushClientMap;
    }
}
