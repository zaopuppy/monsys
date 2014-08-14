package com.letsmidi.monsys.ui;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.letsmidi.monsys.protocol.MonsysServer;
import com.letsmidi.monsys.protocol.MonsysServer.Listener;
import com.letsmidi.monsys.protocol.Push.PushMsg;

public class MonsysActivity extends Activity {
    private static final String TAG = "MonsysActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "onResume(): " + this);
        super.onResume();
        MonsysServer.INSTANCE.setListener(mMonsysListener);
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestory(): " + this);
        super.onDestroy();
    }

    /**
     * to avoid duplicated name in different interface/class
     */
    private final Listener mMonsysListener = new Listener() {
        @Override
        public void onMessage(PushMsg msg) {
            onMonsysMessage(msg);
        }

        @Override
        public void onLoggedIn() {
            onMonsysLoggedIn();
        }

        @Override
        public void onException(Throwable cause) {
            onMonsysException(cause);
        }

        @Override
        public void onDisconnected() {
            onMonsysDisconnected();
        }

        @Override
        public void onConnected() {
            onMonsysConnected();
        }
    };

    private void relogin() {
        Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
    }

    public void onMonsysConnected() {
        Log.d(TAG, "onMonsysConnected()");
    }

    public void onMonsysDisconnected() {
        Log.d(TAG, "onMonsysDisconnected(}");
        MonsysServer.INSTANCE.close();
        relogin();
    }

    public void onMonsysLoggedIn() {
        Log.d(TAG, "onMonsysLoggedIn()");
    }

    public void onMonsysMessage(PushMsg msg) {
        Log.d(TAG, "onMonsysMessage(" + msg.getType() + ")");
    }

    public void onMonsysException(Throwable cause) {
        Log.d(TAG, "onMonsysException(" + cause.toString() + ")");
        MonsysServer.INSTANCE.close();
        relogin();
    }

}

