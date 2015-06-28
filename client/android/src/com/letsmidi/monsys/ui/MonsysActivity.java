package com.letsmidi.monsys.ui;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class MonsysActivity extends Activity {
  private static final String TAG = "MonsysActivity";

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
  }

  @Override
  protected void onResume() {
    Log.d(TAG, "onResume(): " + this);
    super.onResume();
  }

  @Override
  protected void onDestroy() {
    Log.d(TAG, "onDestory(): " + this);
    super.onDestroy();
  }

}

