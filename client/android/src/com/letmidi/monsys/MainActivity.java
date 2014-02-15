package com.letmidi.monsys;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

  private static final String TAG = "XXX";
  private TextView mAccountText = null;
  private TextView mPasswordText = null;
  private Button mLoginBtn = null;

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    Log.d(TAG, "onActivityResult(" + requestCode + ", " + resultCode + ")");
    if (requestCode == 0x00 && resultCode == RESULT_OK) {
      boolean result = data.getBooleanExtra("result", false);
      String account = data.getStringExtra("account");
      Log.d(TAG, "account: " + account + ", result: " + result);

      Intent intent = new Intent(getApplicationContext(), FgwListActivity.class);
      Bundle bundle = new Bundle();
      bundle.putString("account", account);
      intent.putExtras(bundle);

      startActivity(intent);

    } else {
      super.onActivityResult(requestCode, resultCode, data);
    }
  }

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

    mAccountText.setEnabled(false);
    mPasswordText.setEnabled(false);

    mLoginBtn.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View v) {
        Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
        startActivityForResult(intent, 0x00);
      }
    });

    // button1
    Button btn1 = (Button) findViewById(R.id.button1);
//    btn1.setOnClickListener(new OnClickListener() {
//
//      @Override
//      public void onClick(View v) {
//        Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
//        startActivityForResult(intent, 4);
//      }
//    });

    // button2
    Button btn2 = (Button) findViewById(R.id.button2);
//    btn2.setOnClickListener(new OnClickListener() {
//
//      @Override
//      public void onClick(View v) {
//        Thread thread = new Thread() {
//          @Override
//          public void run() {
//            List<FgwInfo> fgw_list = MonsysInterface.getFgwList("ztest@gmail.com");
//            if (fgw_list == null) {
//              Log.d(TAG, "Good!");
//            } else {
//              Log.d(TAG, "NO!");
//            }
//          }
//        };
//        thread.start();
//      }
//    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.main, menu);
    return true;
  }

}
