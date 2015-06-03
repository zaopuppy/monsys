package com.letsmidi.monsys.sdk;



import java.util.HashMap;
import java.util.Map;


public abstract class BaseClient<T> {

    public static interface Callback<T> {
        void onResponse(T msg);
    }

    public static class RouteItem<T> {
        private final int sequence;
        private final Callback<T> callback;

        public RouteItem(int sequence, Callback<T> callback) {
            this.sequence = sequence;
            this.callback = callback;
        }

        public int getSequence() {
            return sequence;
        }

        public Callback<T> getCallback() {
            return callback;
        }
    }

    private final Map<Integer, RouteItem<T>> mRouteMap = new HashMap<>();

    public abstract void connect();

    public abstract boolean isConnected();

    protected void send(T msg, Callback<T> callback) {
        if (!isConnected()) {
            //
        }

        saveRoute(msg, callback);
        // TODO
        // channel.write(msg);
    }

    public Map<Integer, RouteItem<T>> getRouteMap() {
        return mRouteMap;
    }

    protected abstract void saveRoute(T msg, Callback<T> callback);

    protected abstract RouteItem<T> findRoute(T msg);

    private void onResponse(T msg) {
        RouteItem<T> item = findRoute(msg);
        if (item == null) {
            return;
        }

        if (item.getCallback() != null) {
            item.getCallback().onResponse(msg);
        }
    }

}
