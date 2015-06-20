package com.letsmidi.monsys.ui;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import com.letsmidi.monsys.R;


public class BindFgwActivity extends MonsysActivity {
    private static final String TAG = "XXX";

    private EditText mFgwIdEdit;
    private Button mBindButton;

    private Handler mHandler = null;

    private static final int MSG_NOTIFY = 1;

    private class MyHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_NOTIFY:
                    Toast.makeText(BindFgwActivity.this, (CharSequence) msg.obj, Toast.LENGTH_SHORT).show();
                    break;
                default:
                    Log.e(TAG, "unknown message: " + msg.what);
            }
        }

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_bind);

        mHandler = new MyHandler();

        mFgwIdEdit = (EditText) findViewById(R.id.devid_text);
        mBindButton = (Button) findViewById(R.id.bind_button);

        mBindButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                preBindFgw();
            }
        });
    }

    private void preBindFgw() {
        String dev_id = mFgwIdEdit.getText().toString();
        // MonsysHelper.preBindAsync(dev_id, this);
    }

    private void bindFgw() {
        String dev_id = mFgwIdEdit.getText().toString();
        // MonsysHelper.bindAsync(dev_id, this);
    }
}

