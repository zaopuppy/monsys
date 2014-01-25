package com.letmidi.monsys;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.letmidi.monsys.protocol.MonsysInterface;

public class LoginActivity extends Activity {

  private static final String TAG = "LoginActivity";

  private EditText mAccountEdit = null;
  private EditText mPasswordEdit = null;
  private Button mSubmitBtn = null;

  private final LoginTask mLoginTask = new LoginTask();

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
        mLoginTask.execute(
            mAccountEdit.getText().toString(),
            mPasswordEdit.getText().toString());
        disableLogin();
      }

    });
  }

  public class LoginTask extends AsyncTask<String, Integer, Boolean> {
    @Override
    protected Boolean doInBackground(String... params) {
      if (params.length != 2) {
        Log.e(TAG, "Bad argument");
        return false;
      }

      String account = params[0];
      String password = params[1];

      if (account.length() <= 0 || password.length() <= 0) {
        Log.e(TAG, "Bad argument");
        return false;
      }

      return MonsysInterface.login(account, password);
    }

    @Override
    protected void onPostExecute(Boolean result) {
      if (result) {
        onLoginSuccess();
      } else {
        onLoginFail();
      }
    }
  }

  private void onLoginSuccess() {
    enableLogin();
    Toast.makeText(getApplicationContext(), "login success :)", Toast.LENGTH_SHORT).show();
  }

  private void onLoginFail() {
    enableLogin();
    Toast.makeText(getApplicationContext(), "login failed :(", Toast.LENGTH_SHORT).show();
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

}
