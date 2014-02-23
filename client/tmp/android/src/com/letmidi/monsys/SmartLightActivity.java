package com.letmidi.monsys;


import java.util.LinkedList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Toast;

import com.letmidi.monsys.protocol.MonsysInterface;

public class SmartLightActivity extends Activity implements OnSeekBarChangeListener {

  private static final String TAG = "XXX";

  private static final int ID_COLOR_R = 1;
  private static final int ID_COLOR_G = 2;
  private static final int ID_COLOR_B = 3;

  private Button mRefreshButton;
  // color picker
  private SurfaceView mColorPickView;
  private SurfaceHolder mColorPickViewHolder;

  // R'G'B' sliders
  private SeekBar mRSeekBar;
  private SeekBar mGSeekBar;
  private SeekBar mBSeekBar;

  private String mFgwId;
  private int mDevAddr;

  private QueryDevInfoTask mQueryTask = new QueryDevInfoTask();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_smartlight);

    // get parameters
    Intent intent = getIntent();
    Bundle bundle = intent.getExtras();
    mFgwId = bundle.getString("fgw-id");
    mDevAddr = bundle.getInt("dev-addr", -1);
    if (mFgwId == null || mFgwId.length() <= 0 || mDevAddr == -1) {
      AlertDialog.Builder builder = new AlertDialog.Builder(this);
      builder
          .setTitle("Bubu!")
          .setMessage("No fgw-id or dev-addr was supplied")
          .setPositiveButton("Fine", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              finish();
            }
          })
          .show();
    } else {
      mRefreshButton = (Button) findViewById(R.id.refresh_button);
      mRefreshButton.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
          if (mQueryTask != null) {
            mQueryTask.cancel(true);
          }
          mQueryTask = new QueryDevInfoTask();
          mQueryTask.mIds.add(0); // query all id
          mQueryTask.execute();
        }
      });

      mColorPickView = (SurfaceView) findViewById(R.id.color_pick_view);
      mColorPickViewHolder = mColorPickView.getHolder();

      mRSeekBar = (SeekBar) findViewById(R.id.r_slider);
      mGSeekBar = (SeekBar) findViewById(R.id.g_slider);
      mBSeekBar = (SeekBar) findViewById(R.id.b_slider);

      mQueryTask = new QueryDevInfoTask();
      mQueryTask.mIds.add(0); // query all id
      mQueryTask.execute();
    }

    //
    initUIComponents();
  }

  private void initUIComponents() {
//    mRSeekBar.setEnabled(false);
//    mGSeekBar.setEnabled(false);
//    mBSeekBar.setEnabled(false);

    mRSeekBar.setOnSeekBarChangeListener(this);
    mGSeekBar.setOnSeekBarChangeListener(this);
    mBSeekBar.setOnSeekBarChangeListener(this);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
  }

  @Override
  public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
//    Log.d(TAG, "onProgressChanged(" + progress + ")");
  }

  @Override
  public void onStartTrackingTouch(SeekBar seekBar) {
    Log.d(TAG, "onStartTrackingTouch(" + seekBar.getProgress() + ")");
  }

  @Override
  public void onStopTrackingTouch(SeekBar seekBar) {
    Log.d(TAG, "onStopTrackingTouch(" + seekBar.getProgress() + ")");
  }

  private class QueryDevInfoTask extends AsyncTask<Void, Integer, List<Pair<Integer, Integer>>> {

    public List<Integer> mIds = new LinkedList<Integer>();

    @Override
    protected List<Pair<Integer, Integer>> doInBackground(Void... params) {

      List<Pair<Integer, Integer>> info_list = MonsysInterface.getDevInfo(mFgwId, mDevAddr, mIds);
      if (info_list == null) {
        Log.e(TAG, "Failed to get dev info");
        return null;
      }

      return info_list;
    }

    @Override
    protected void onPostExecute(List<Pair<Integer, Integer>> result) {
      if (result == null) {
        Log.e(TAG, "Failed to get result");
        Toast.makeText(getApplicationContext(), "failed to get dev info", Toast.LENGTH_SHORT).show();
        return;
      }

      for (Pair<Integer, Integer> pair: result) {
        if (pair.first == ID_COLOR_R) {    // R
          Log.d(TAG, "Enable R-SeekBar");
          mRSeekBar.setEnabled(true);
          mRSeekBar.setProgress(mapColorValue(pair.second));
        } else if (pair.first == ID_COLOR_G) {
          mGSeekBar.setEnabled(true);
          mGSeekBar.setProgress(mapColorValue(pair.second));
        } else if (pair.first == ID_COLOR_B) {
          mBSeekBar.setEnabled(true);
          mBSeekBar.setProgress(mapColorValue(pair.second));
        } else {
          Log.e(TAG, "Unknown id: " + pair.first + "=" + pair.second);
        }
      }
      Toast.makeText(getApplicationContext(), "dev info got successfully", Toast.LENGTH_SHORT).show();
    }

  }

  private int mapColorValue(Integer value) {
    if (value > mRSeekBar.getMax()) {
      Log.w(TAG, "bigger than max value");
      return mRSeekBar.getMax();
    } else if (value < 0) {
      return 0;
    } else {
      return value;
    }
  }
}
