package com.letsmidi.monsys;

import android.app.Application;
import android.util.Log;
import com.letsmidi.monsys.util.DataStore;

public class MonsysClientApp extends Application {

  private static String TAG = "XXX";

  @Override
  public void onCreate() {
    Log.d("XXX", "MonsysClientApp.onCreate()");
    super.onCreate();
    DataStore.init(getApplicationContext());
  }

}
