package com.letmidi.monsys;

import android.app.Application;

public class MonsysClientApp extends Application {

  @Override
  public void onCreate() {
    Config.setInstance(getApplicationContext());
    super.onCreate();
  }

}
