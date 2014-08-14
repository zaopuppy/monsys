package com.letsmidi.monsys;

import android.app.Application;

public class MonsysClientApp extends Application {

  @Override
  public void onCreate() {
    ConfigManager.setInstance(getApplicationContext());
    super.onCreate();
  }

}
