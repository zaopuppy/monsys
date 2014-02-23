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

//  private final LoginTask mLoginTask = new LoginTask();
  private static final int MSG_ON_LOGIN = 0x01;

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
        case MSG_ON_LOGIN:
          Boolean result = (Boolean) msg.obj;
          Intent data = new Intent();
          if (result) {
            Log.d(TAG, "Seikou");
          } else {
            Log.d(TAG, "Shibbai");
          }
          data.putExtra("result", result);
          setResult(RESULT_OK, data);
          break;
      }
    }
  };

  private void test() {

  }

  @Override
  public void onLogin(boolean result) {
    Message msg = sHandler.obtainMessage(MSG_ON_LOGIN, result);
    msg.sendToTarget();
  }

//  public class LoginTask extends AsyncTask<String, Integer, Boolean> {
//    @Override
//    protected Boolean doInBackground(String... params) {
//      if (params.length != 2) {
//        Log.e(TAG, "Bad argument");
//        return false;
//      }
//
//      String account = params[0];
//      String password = params[1];
//
//      if (account.length() <= 0 || password.length() <= 0) {
//        Log.e(TAG, "Bad argument");
//        return false;
//      }
//
//      return MonsysInterface.login(account, password);
//    }
//
//    @Override
//    protected void onPostExecute(Boolean result) {
//      if (result) {
//        onLoginSuccess();
//      } else {
//        onLoginFail();
//      }
//    }
//  }
//
//  private void onLoginSuccess() {
//    enableLogin();
//    Toast.makeText(getApplicationContext(), "login success :)", Toast.LENGTH_SHORT).show();
//  }
//
//  private void onLoginFail() {
//    enableLogin();
//    Toast.makeText(getApplicationContext(), "login failed :(", Toast.LENGTH_SHORT).show();
//  }

}
