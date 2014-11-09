package com.letsmidi.monsys.util;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by zero on 8/24/14.
 */
public class DataStore {

  private static SharedPreferences mSharedPreferences = null;

  public static boolean init(Context context) {
    mSharedPreferences = context.getSharedPreferences("monsys", Context.MODE_PRIVATE);
    return true;
  }

  public static void saveString(String key, String value) {
    SharedPreferences.Editor editor = mSharedPreferences.edit();
    editor.putString(key, value);
    editor.commit();
  }

  public static String getString(String key, String default_value) {
    return mSharedPreferences.getString(key, default_value);
  }

  public static void saveInt(String key, int value) {
    SharedPreferences.Editor editor = mSharedPreferences.edit();
    editor.putInt(key, value);
    editor.commit();
  }

  public static int getInt(String key, int default_value) {
    return mSharedPreferences.getInt(key, default_value);
  }
}
