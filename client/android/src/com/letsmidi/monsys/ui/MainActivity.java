package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.sdk.MonsysClient;

public class MainActivity extends MonsysActivity {

    private static final String TAG = "XXX";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        Button showFgwBtn = (Button) findViewById(R.id.show_gw_list_button);
        Button logoutBtn = (Button) findViewById(R.id.logout_button);

        showFgwBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(getApplicationContext(), FgwListActivity.class);
                startActivity(intent);
            }
        });

        logoutBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MonsysClient.getConnection().close();
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
                //MonsysServer.INSTANCE.setListener(null);
                MonsysClient.getConnection().close();
                Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
                startActivity(intent);
                //android.os.Process.killProcess(Process.myPid());
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

        if (!MonsysClient.getConnection().isLoggedIn()) {
            Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
            startActivity(intent);
        }
    }

}
