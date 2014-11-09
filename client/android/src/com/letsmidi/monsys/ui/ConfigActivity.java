package com.letsmidi.monsys.ui;

import android.app.Activity;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import com.letsmidi.monsys.R;

/**
 * Created by zero on 8/24/14.
 */
public class ConfigActivity extends Activity {
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    getFragmentManager()
        .beginTransaction()
        .replace(android.R.id.content, new ConfigFragment())
        .commit();
  }
}