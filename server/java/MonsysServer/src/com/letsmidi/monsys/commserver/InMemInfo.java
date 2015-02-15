package com.letsmidi.monsys.commserver;

/**
 * Created by z00302892 on 2015/2/15.
 */
public class InMemInfo {
    public static final InMemInfo INSTANCE = new InMemInfo();

    private volatile boolean loggedIn = false;

    public boolean isLoggedIn() {
        return loggedIn;
    }

    public void setLoggedIn(boolean loggedIn) {
        this.loggedIn = loggedIn;
    }
}
