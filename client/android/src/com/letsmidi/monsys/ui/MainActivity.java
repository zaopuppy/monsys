package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;

public class MainActivity extends MonsysActivity {

    private static final String TAG = "XXX";
    private Button mShowFgwBtn = null;
    private Button mLogoutBtn = null;

    private void startFgwListActivity() {
        Intent intent = new Intent(getApplicationContext(), FgwListActivity.class);
        // Bundle bundle = new Bundle();
        // bundle.putString("account", account);
        // intent.putExtras(bundle);
        startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        mShowFgwBtn = (Button) findViewById(R.id.login_button);
        mLogoutBtn = (Button) findViewById(R.id.logout_button);

        mShowFgwBtn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (MonsysServer.INSTANCE.isLoggedIn()) {
                    startFgwListActivity();
                } else {
                    Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
                    startActivity(intent);
                }
            }
        });

        mLogoutBtn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // MonsysHelper.logout();
                MonsysServer.INSTANCE.close();
            }
        });

        // button1
        Button btn1 = (Button) findViewById(R.id.button1);
        btn1.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // Intent intent = new Intent(getApplicationContext(),
                // LoginActivity.class);
                // startActivityForResult(intent, 4);
                Intent intent = new Intent(getApplicationContext(), BindFgwActivity.class);
                // Bundle bundle = new Bundle();
                // bundle.putString("fgw-id", "DEVID-Z");
                // intent.putExtras(bundle);
                startActivity(intent);
            }
        });

        // button2
        Button btn2 = (Button) findViewById(R.id.button2);
        // btn2.setOnClickListener(new OnClickListener() {
        //
        // @Override
        // public void onClick(View v) {
        // Thread thread = new Thread() {
        // @Override
        // public void run() {
        // List<FgwInfo> fgw_list =
        // MonsysInterface.getFgwList("ztest@gmail.com");
        // if (fgw_list == null) {
        // Log.d(TAG, "Good!");
        // } else {
        // Log.d(TAG, "NO!");
        // }
        // }
        // };
        // thread.start();
        // }
        // });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    // @Override
    // protected void onActivityResult(int requestCode, int resultCode, Intent
    // data) {
    // Log.d(TAG, "onActivityResult(" + requestCode + ", " + resultCode + ")");
    // if (requestCode == 0x00 && resultCode == RESULT_OK) {
    // boolean result = data.getBooleanExtra("result", false);
    // String account = data.getStringExtra("account");
    // Log.d(TAG, "account: " + account + ", result: " + result);
    //
    // startFgwListActivity(account);
    // } else {
    // super.onActivityResult(requestCode, resultCode, data);
    // }
    // }

    @Override
    protected void onResume() {
        super.onResume();

        // if (!MonsysHelper.isLoggedIn()) {
        if (!MonsysServer.INSTANCE.isLoggedIn()) {
            Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
            startActivity(intent);
        }
    }

}
