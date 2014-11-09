package com.letsmidi.monsys.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;
import com.letsmidi.monsys.protocol.push.Push;

public class LoginActivity extends MonsysActivity {
  private static final String TAG = "XXX";

  private EditText mAccountEdit = null;
  private EditText mPasswordEdit = null;
  private Button mSubmitBtn = null;
  private Button mRegisterButton = null;

  private static final int MSG_LOGIN_COMPLETE = 0x01;

  private enum State {
    None,
    Login,
    Register,
  }

  private State mState = State.None;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_login);

    mAccountEdit = (EditText) findViewById(R.id.account_text);
    mPasswordEdit = (EditText) findViewById(R.id.password_text);
    mSubmitBtn = (Button) findViewById(R.id.login_button);
    mRegisterButton = (Button) findViewById(R.id.register_button);

    mSubmitBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        disableLogin();

        mState = State.Login;

        // TODO: move close() inside of connect()
        // close it first
        MonsysServer.INSTANCE.close();
        MonsysServer.INSTANCE.connect();
      }
    });

    mRegisterButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        disableLogin();

        Log.d(TAG, "register account+password: ["
            + mAccountEdit.getText().toString() + "]:["
            + mPasswordEdit.getText().toString() + "]");

        mState = State.Register;

        // TODO: move close() inside of connect()
        MonsysServer.INSTANCE.close();
        MonsysServer.INSTANCE.connect();
      }
    });
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case 0: {
        Intent intent = new Intent(getApplicationContext(), ConfigActivity.class);
        startActivity(intent);
        return true;
      }
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    menu.add(0, 0, 0, "Configure");
    return super.onCreateOptionsMenu(menu);
  }

  private void enableLogin() {
    mAccountEdit.setEnabled(true);
    mPasswordEdit.setEnabled(true);
    mSubmitBtn.setEnabled(true);
    mRegisterButton.setEnabled(true);
  }

  private void disableLogin() {
    mAccountEdit.setEnabled(false);
    mPasswordEdit.setEnabled(false);
    mSubmitBtn.setEnabled(false);
    mRegisterButton.setEnabled(false);
  }

  private final Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_LOGIN_COMPLETE:
        {
          Boolean result = (Boolean) msg.obj;
          if (result) {
            Log.d(TAG, "Seikou");
            finish();
          } else {
            Log.d(TAG, "Shibbai");
            enableLogin();
          }
          break;
        }
        default:
          Log.w(TAG, "Unknown msg.what: " + msg.what);
          break;
      }
    }
  };

  @Override
  protected void onResume() {
    super.onResume();
    mState = State.None;
  }

  @Override
  public void onMonsysConnected() {
    super.onMonsysConnected();

    switch (mState) {
      case Login:
        MonsysServer.INSTANCE.clientLogin(
            mAccountEdit.getText().toString(),
            mPasswordEdit.getText().toString());
        break;
      case Register:
        MonsysServer.INSTANCE.userRegister(
            mAccountEdit.getText().toString(),
            mPasswordEdit.getText().toString());
        break;
      default:
        Log.e(TAG, "Unknown state: " + mState);
        break;
    }
  }

  @Override
  public void onMonsysMessage(Push.PushMsg msg) {
    super.onMonsysMessage(msg);

    switch (msg.getType()) {
      case CLIENT_LOGIN_RSP: {
        if (!msg.hasClientLoginRsp()) {
          Log.e(TAG, "bad response, close");
          MonsysServer.INSTANCE.close();
          return;
        }
        Push.ClientLoginRsp rsp = msg.getClientLoginRsp();
        if (rsp.getCode() != 0) {
          Log.e(TAG, "bad response: " + rsp.getCode());
          MonsysServer.INSTANCE.close();
          return;
        }
        Message inner_msg = mHandler.obtainMessage(MSG_LOGIN_COMPLETE, true);
        inner_msg.sendToTarget();
        break;
      }
      case USER_REGISTER_RSP: {
        if (!msg.hasUserRegisterRsp()) {
          Toast.makeText(
              getApplicationContext(),
              "Bad message, user register response is expect",
              Toast.LENGTH_SHORT)
              .show();
          return;
        }

        Push.UserRegisterRsp rsp = msg.getUserRegisterRsp();
        if (rsp.getCode() != 0) {
          Toast.makeText(
              getApplicationContext(),
              "Failed, code is " + rsp.getCode(),
              Toast.LENGTH_SHORT)
              .show();
          return;
        }

        // success, login immediately
        //MonsysServer.INSTANCE.close();
        //MonsysServer.INSTANCE.connect(mAccountEdit.getText().toString(), mPasswordEdit.getText().toString());
        MonsysServer.INSTANCE.clientLogin(
            mAccountEdit.getText().toString(),
            mPasswordEdit.getText().toString());

        break;
      }
      default: {
        Toast.makeText(
            getApplicationContext(),
            "Bad message: " + msg.getType(),
            Toast.LENGTH_SHORT)
            .show();
        break;
      }
    }
  }
}
