package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;

public class MainActivity extends MonsysActivity {

  private static final String TAG = "XXX";
  private Button mShowFgwBtn = null;
  private Button mLogoutBtn = null;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_main);

    mShowFgwBtn = (Button) findViewById(R.id.login_button);
    mLogoutBtn = (Button) findViewById(R.id.logout_button);

    mShowFgwBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        //if (MonsysServer.INSTANCE.isLoggedIn()) {
        Intent intent = new Intent(getApplicationContext(), FgwListActivity.class);
        startActivity(intent);
        //} else {
        //  Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
        //  startActivity(intent);
        //}
      }
    });

    mLogoutBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        MonsysServer.INSTANCE.close();
      }
    });

    // button1
    Button btn1 = (Button) findViewById(R.id.button1);
    btn1.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        Intent intent = new Intent(getApplicationContext(), BindFgwActivity.class);
        startActivity(intent);
      }
    });

    // button2
    Button btn2 = (Button) findViewById(R.id.button2);
    btn2.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        MonsysServer.INSTANCE.setListener(null);
        MonsysServer.INSTANCE.close();
        finish();
        android.os.Process.killProcess(Process.myPid());
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.main, menu);
    return true;
  }

  @Override
  protected void onResume() {
    super.onResume();

    if (!MonsysServer.INSTANCE.isConnected()) {
      Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
      startActivity(intent);
    }
  }

}
