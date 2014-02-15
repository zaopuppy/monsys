package com.letmidi.monsys;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.letmidi.monsys.account.AccountManager;
import com.letmidi.monsys.account.AccountManager.LoginCallback;

public class LoginActivity extends Activity implements LoginCallback {

  private static final String TAG = "XXX";

  private EditText mAccountEdit = null;
  private EditText mPasswordEdit = null;
  private Button mSubmitBtn = null;

  private static final int MSG_LOGIN_COMPLETE = 0x01;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_login);

    mAccountEdit = (EditText) findViewById(R.id.account_text);
    mPasswordEdit = (EditText) findViewById(R.id.password_text);
    mSubmitBtn = (Button) findViewById(R.id.login_button);
    mSubmitBtn.setOnClickListener(new View.OnClickListener() {

      @Override
      public void onClick(View v) {
        disableLogin();
        AccountManager.loginAsync(mAccountEdit.getText().toString(),
                                  mPasswordEdit.getText().toString(),
                                  LoginActivity.this);
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

  private final Handler sHandler = new Handler() {

    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_LOGIN_COMPLETE:
          Boolean result = (Boolean) msg.obj;
          if (result) {
            Log.d(TAG, "Seikou");
          } else {
            Log.d(TAG, "Shibbai");
          }
          Intent data = new Intent();
          data.putExtra("result", result);
          data.putExtra("account", mAccountEdit.getText().toString());
          setResult(RESULT_OK, data);
          LoginActivity.this.finish();
          break;
      }
    }
  };

  private void test() {

  }

  @Override
  public void onLogin(boolean result) {
    Message msg = sHandler.obtainMessage(MSG_LOGIN_COMPLETE, result);
    msg.sendToTarget();
  }

}
