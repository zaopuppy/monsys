package com.letsmidi.monsys;

import android.content.Context;
import android.content.SharedPreferences;

public class ConfigManager {
  private static final String TAG = "Config";

  private static final String CONFIG_COMMON = "common";

  private final Context mContext;

  private static ConfigManager mInstance = null;

  private ConfigManager(Context ctx) {
    mContext = ctx;
  }

  public static void setInstance(Context ctx) {
    mInstance = new ConfigManager(ctx);
  }
  public static ConfigManager instance() {
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
