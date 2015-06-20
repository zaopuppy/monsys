package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.sdk.MonsysClient;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.GenericFutureListener;

public class LoginActivity extends MonsysActivity {
    private static final String TAG = "XXX";

    private EditText mAccountEdit = null;
    private EditText mPasswordEdit = null;
    private Button mSubmitBtn = null;
    private Button mRegisterButton = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        mAccountEdit = (EditText) findViewById(R.id.account_text);
        mPasswordEdit = (EditText) findViewById(R.id.password_text);
        mSubmitBtn = (Button) findViewById(R.id.login_button);
        mRegisterButton = (Button) findViewById(R.id.register_button);

        mSubmitBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                disableLogin();

                // TODO: move close() inside of connect()
                // close it first
                MonsysClient.getConnection().close();
                MonsysClient.getConnection().login(
                    mAccountEdit.getText().toString(),
                    mPasswordEdit.getText().toString()).addListener(new GenericFutureListener<Future<Integer>>() {
                    @Override
                    public void operationComplete(Future<Integer> future) throws Exception {
                        try {
                            final int code = future.get();
                            if (code == 0) {
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(getApplicationContext(), "succeed", Toast.LENGTH_SHORT).show();
                                        finish();
                                    }
                                });
                            } else {
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(getApplicationContext(), "failed: " + code, Toast.LENGTH_SHORT).show();
                                    }
                                });
                            }
                        } finally {
                            enableLogin();
                        }
                    }
                });
            }
        });

        //mRegisterButton.setOnClickListener(new View.OnClickListener() {
        //  @Override
        //  public void onClick(View v) {
        //    disableLogin();
        //
        //    Log.d(TAG, "register account+password: ["
        //        + mAccountEdit.getText().toString() + "]:["
        //        + mPasswordEdit.getText().toString() + "]");
        //
        //    mState = State.Register;
        //
        //    // TODO: move close() inside of connect()
        //    MonsysServer.INSTANCE.close();
        //    MonsysServer.INSTANCE.connect();
        //  }
        //});
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case 0: {
                Intent intent = new Intent(getApplicationContext(), ConfigActivity.class);
                startActivity(intent);
                return true;
            }
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, 0, 0, "Configure");
        return super.onCreateOptionsMenu(menu);
    }

    private void enableLogin() {
        mAccountEdit.setEnabled(true);
        mPasswordEdit.setEnabled(true);
        mSubmitBtn.setEnabled(true);
        mRegisterButton.setEnabled(true);
    }

    private void disableLogin() {
        mAccountEdit.setEnabled(false);
        mPasswordEdit.setEnabled(false);
        mSubmitBtn.setEnabled(false);
        mRegisterButton.setEnabled(false);
    }

}

