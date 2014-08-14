package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;

public class LoginActivity extends MonsysActivity {

    private static final String TAG = "XXX";

    private EditText mAccountEdit = null;
    private EditText mPasswordEdit = null;
    private Button mSubmitBtn = null;
    private Button mRegisterBtn = null;

    private static final int MSG_LOGIN_COMPLETE = 0x01;

    private class MyAsyncTask extends AsyncTask<String, Integer, String> {
        @Override
        protected String doInBackground(String... params) {
            return null;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        mAccountEdit = (EditText) findViewById(R.id.account_text);
        mPasswordEdit = (EditText) findViewById(R.id.password_text);
        mSubmitBtn = (Button) findViewById(R.id.login_button);
        mRegisterBtn = (Button) findViewById(R.id.register_button);

        mSubmitBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                disableLogin();

                // close it first
                MonsysServer.INSTANCE.close();
                MonsysServer.INSTANCE.connect(mAccountEdit.getText().toString(),
                                              mPasswordEdit.getText().toString());
            }
        });

        mRegisterBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "account+password: ["
                        + mAccountEdit.getText().toString() + "]:["
                        + mPasswordEdit.getText().toString() + "]");
            }
        });
    }

    private void enableLogin() {
        mAccountEdit.setEnabled(true);
        mPasswordEdit.setEnabled(true);
        mSubmitBtn.setEnabled(true);
    }

    private void disableLogin() {
        mAccountEdit.setEnabled(false);
        mPasswordEdit.setEnabled(false);
        mSubmitBtn.setEnabled(false);
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_LOGIN_COMPLETE:
                {
                    Boolean result = (Boolean) msg.obj;
                    if (result) {
                        Log.d(TAG, "Seikou");
                        Intent data = new Intent();
                        data.putExtra("result", result);
                        data.putExtra("account", mAccountEdit.getText().toString());
                        setResult(RESULT_OK, data);
                        LoginActivity.this.finish();
                    } else {
                        Log.d(TAG, "Shibbai");
                        enableLogin();
                    }
                    break;
                }
                default:
                    Log.w(TAG, "Unknown msg.what: " + msg.what);
                    break;
            }
        }
    };

    @Override
    public void onMonsysLoggedIn() {
        Message inner_msg = mHandler.obtainMessage(MSG_LOGIN_COMPLETE, true);
        inner_msg.sendToTarget();
    }

}
