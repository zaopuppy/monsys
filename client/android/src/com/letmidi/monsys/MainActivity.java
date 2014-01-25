package com.letmidi.monsys;

import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.letmidi.monsys.protocol.MonsysInterface;

public class MainActivity extends Activity {

  private static final String TAG = "XXX";
  private TextView mAccountText = null;
  private TextView mPasswordText = null;
  private Button mLoginBtn = null;
  private LoginTask mLoginTask;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // check if we have logged in
//    String cookies = Config.instance().getString("cookies", null);
//    if (cookies != null) {
//      // logged in
//      //
//    }

    setContentView(R.layout.activity_main);

    mAccountText = (TextView) findViewById(R.id.account_text);
    mPasswordText = (TextView) findViewById(R.id.password_text);
    mLoginBtn = (Button) findViewById(R.id.login_button);

    mLoginBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {
        if (mLoginTask != null) {
          mLoginTask.cancel(true);
        }
        mLoginTask = new LoginTask();
        mLoginTask.execute(
            mAccountText.getText().toString(), mPasswordText.getText().toString());
      }
    });

    Button btn1 = (Button) findViewById(R.id.button1);
    btn1.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        Thread thread = new Thread() {
          @Override
          public void run() {
            if (MonsysInterface.login("ztest@gmail.com", "123")) {
              Log.d(TAG, "Good!");
//              Toast.makeText(getApplicationContext(), "Good!", Toast.LENGTH_SHORT).show();
            } else {
              Log.d(TAG, "NO!");
//              Toast.makeText(getApplicationContext(), "NO!", Toast.LENGTH_SHORT).show();
            }
          }
        };
        thread.start();
      }
    });

    Button btn2 = (Button) findViewById(R.id.button2);
    btn2.setOnClickListener(new OnClickListener() {

      @Override
      public void onClick(View v) {
        Thread thread = new Thread() {
          @Override
          public void run() {
            List<FgwInfo> fgw_list = MonsysInterface.getFgwList("ztest@gmail.com");
            if (fgw_list == null) {
//              Toast.makeText(getApplicationContext(), "NO!", Toast.LENGTH_SHORT).show();
              Log.d(TAG, "Good!");
            } else {
//              Toast.makeText(getApplicationContext(), "Good!", Toast.LENGTH_SHORT).show();
              Log.d(TAG, "NO!");
            }
          }
        };
        thread.start();
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.main, menu);
    return true;
  }

  private class LoginTask extends AsyncTask<String, Integer, Boolean> {

    @Override
    protected Boolean doInBackground(String... params) {

      String account = params[0];
      String password = params[1];

      if (!MonsysInterface.login(account, password)) {
        Log.e("XXX", "Failed to login monsys with ([" + account + "], [" + password + "])");
        return false;
      }

      return true;
    }

    @Override
    protected void onPostExecute(Boolean result) {
      if (!result) {
        Log.e("XXX", "task failed");
        Toast.makeText(getApplicationContext(), "Failed to login :(", Toast.LENGTH_SHORT).show();
        return;
      }

      Toast.makeText(getApplicationContext(), "login success", Toast.LENGTH_SHORT).show();

      Bundle bundle = new Bundle();
      bundle.putString("account", mAccountText.getText().toString());
//      bundle.putString("password", mPasswordText.getText().toString());

      Intent intent = new Intent(getApplicationContext(), FgwListActivity.class);
      intent.putExtras(bundle);

      startActivity(intent);
    }

  }

}
