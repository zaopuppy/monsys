package com.letsmidi.monsys.ui;

import android.os.Bundle;
import android.preference.PreferenceFragment;
import com.letsmidi.monsys.R;

/**
 * Created by zero on 8/24/14.
 */
public class ConfigFragment extends PreferenceFragment {
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    addPreferencesFromResource(R.xml.preference_config);
  }
}
