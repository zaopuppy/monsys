package com.letmidi.monsys;

import android.content.Context;
import android.content.SharedPreferences;

public class Config {
  private static final String TAG = "Config";

  private static final String CONFIG_COMMON = "common";

  private final Context mContext;

  private static Config mInstance = null;

  private Config(Context ctx) {
    mContext = ctx;
  }

  public static void setInstance(Context ctx) {
    mInstance = new Config(ctx);
  }
  public static Config instance() {
    return mInstance;
  }

  public SharedPreferences getsp() {
    return mContext.getSharedPreferences(CONFIG_COMMON, Context.MODE_PRIVATE);
  }

  public String getString(String key, String dft_val) {
    return getsp().getString(key, dft_val);
  }

  public int getInt(String key, int dft_val) {
    return getsp().getInt(key, dft_val);
  }
}
