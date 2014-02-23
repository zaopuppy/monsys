package com.letmidi.monsys;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.letmidi.monsys.account.AccountManager;
import com.letmidi.monsys.account.AccountManager.BindCallback;
import com.letmidi.monsys.account.AccountManager.PreBindCallback;

public class BindFgwActivity extends Activity implements PreBindCallback, BindCallback {
  private static final String TAG = "XXX";

  private EditText mFgwIdEdit;
  private Button mBindButton;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_bind);

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
    AccountManager.preBindAsync(dev_id, this);
  }

  private void bindFgw() {
    String dev_id = mFgwIdEdit.getText().toString();
    AccountManager.bindAsync(dev_id, this);
  }

  @Override
  public void onPreBind(boolean result) {
    Log.d(TAG, "onPreBind(" + result + ")");
    if (result) {
      bindFgw();
    }
  }

  @Override
  public void onBind(boolean result) {
    Log.d(TAG, "onBind(" + result + ")");
  }

}
