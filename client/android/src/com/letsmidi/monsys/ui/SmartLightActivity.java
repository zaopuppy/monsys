package com.letsmidi.monsys.ui;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Canvas;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Toast;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;
import com.letsmidi.monsys.protocol.push.Push;

import java.util.LinkedList;
import java.util.List;

public class SmartLightActivity extends MonsysActivity implements OnSeekBarChangeListener, SurfaceHolder.Callback {

  private static final String TAG = "XXX";

  private static final int ID_COLOR_R = 1;
  private static final int ID_COLOR_G = 2;
  private static final int ID_COLOR_B = 3;

  private SurfaceHolder mColorPickViewHolder;

  // R'G'B' sliders
  private SeekBar mRSeekBar;
  private SeekBar mGSeekBar;
  private SeekBar mBSeekBar;

  // private String mFgwId;
  private int mDevAddr;

  private static final int MSG_GET_DEV_INFO_LIST_COMPLETE = 0x01;

  private final Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_GET_DEV_INFO_LIST_COMPLETE:
          List<Pair<Integer, Integer>> dev_info_list = (List<Pair<Integer, Integer>>) msg.obj;
          handleGetDevInfoResult(dev_info_list);
          break;
      }
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_smartlight);

    // get parameters
    Intent intent = getIntent();
    Bundle bundle = intent.getExtras();
    mDevAddr = bundle.getInt("dev-addr", -1);
    if (mDevAddr < 0) {
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
    }

    //
    initUIComponents();
  }

  private void handleGetDevInfoResult(List<Pair<Integer, Integer>> result) {
    if (result == null) {
      Log.e(TAG, "Failed to get result");
      Toast.makeText(getApplicationContext(), "failed to get dev info", Toast.LENGTH_SHORT).show();
      return;
    }

    for (Pair<Integer, Integer> pair : result) {
      if (pair.first == ID_COLOR_R) { // R
        Log.d(TAG, "Enable R-SeekBar: " + pair.second);
        mRSeekBar.setEnabled(true);
        mRSeekBar.setProgress(mapColorValue(pair.second));
      } else if (pair.first == ID_COLOR_G) {
        Log.d(TAG, "Enable G-SeekBar: " + pair.second);
        mGSeekBar.setEnabled(true);
        mGSeekBar.setProgress(mapColorValue(pair.second));
      } else if (pair.first == ID_COLOR_B) {
        Log.d(TAG, "Enable B-SeekBar: " + pair.second);
        mBSeekBar.setEnabled(true);
        mBSeekBar.setProgress(mapColorValue(pair.second));
      } else {
        Log.e(TAG, "Unknown id: " + pair.first + "=" + pair.second);
      }
    }

    setPreview(mRSeekBar.getProgress(), mGSeekBar.getProgress(), mBSeekBar.getProgress());

    Toast.makeText(getApplicationContext(), "dev info got successfully", Toast.LENGTH_SHORT).show();
  }

  private void setPreview(int r, int g, int b) {
    Canvas canvas = mColorPickViewHolder.lockCanvas();
    try {
      canvas.drawRGB(r, g, b);
    } finally {
      mColorPickViewHolder.unlockCanvasAndPost(canvas);
    }
  }

  private void initUIComponents() {
    Button mRefreshButton = (Button) findViewById(R.id.refresh_button);
    mRefreshButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        List<Integer> id_list = new LinkedList<>();
        id_list.add(0);
        MonsysServer.INSTANCE.getDevInfo(mDevAddr, id_list);
      }
    });

    SurfaceView mColorPickView = (SurfaceView) findViewById(R.id.color_pick_view);
    mColorPickViewHolder = mColorPickView.getHolder();
    mColorPickViewHolder.addCallback(this);

    mRSeekBar = (SeekBar) findViewById(R.id.r_slider);
    mGSeekBar = (SeekBar) findViewById(R.id.g_slider);
    mBSeekBar = (SeekBar) findViewById(R.id.b_slider);

    Button mSwitchButton = (Button) findViewById(R.id.switch_button);
    mSwitchButton.setOnClickListener(new View.OnClickListener() {

      @Override
      public void onClick(View v) {
        List<Pair<Integer, Integer>> id_vals = new LinkedList<>();
        id_vals.add(new Pair<>(ID_COLOR_R, 0));
        id_vals.add(new Pair<>(ID_COLOR_B, 0));
        id_vals.add(new Pair<>(ID_COLOR_G, 0));
        setPreview(0, 0, 0);
        MonsysServer.INSTANCE.setDevInfo(mDevAddr, id_vals);
      }
    });

    List<Integer> id_list = new LinkedList<>();
    id_list.add(0);
    MonsysServer.INSTANCE.getDevInfo(mDevAddr, id_list);

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
    // Log.d(TAG, "onProgressChanged(" + progress + ")");
    setPreview(mRSeekBar.getProgress(), mGSeekBar.getProgress(), mBSeekBar.getProgress());
  }

  @Override
  public void onStartTrackingTouch(SeekBar seekBar) {
    Log.d(TAG, "onStartTrackingTouch(" + seekBar.getProgress() + ")");
  }

  @Override
  public void onStopTrackingTouch(SeekBar seekBar) {
    Log.d(TAG, "onStopTrackingTouch(" + seekBar.getProgress() + ")");

    List<Pair<Integer, Integer>> id_vals = new LinkedList<>();

    if (seekBar == mRSeekBar) {
      id_vals.add(new Pair<>(ID_COLOR_R, seekBar.getProgress()));
    } else if (seekBar == mGSeekBar) {
      id_vals.add(new Pair<>(ID_COLOR_G, seekBar.getProgress()));
    } else if (seekBar == mBSeekBar) {
      id_vals.add(new Pair<>(ID_COLOR_B, seekBar.getProgress()));
    } else {
      Log.e(TAG, "unknown seekbar");
      return;
    }

    MonsysServer.INSTANCE.setDevInfo(mDevAddr, id_vals);
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

  @Override
  public void onMonsysMessage(Push.PushMsg msg) {
    if (msg.getType() == Push.MsgType.GET_DEV_INFO_RSP && msg.hasGetDevInfoRsp()) {
      Log.d(TAG, "get_dev_info_rsp received");
      Push.GetDevInfoRsp rsp = msg.getGetDevInfoRsp();
      List<Pair<Integer, Integer>> dev_info_list = new LinkedList<>();

      for (Push.IdValuePair info: rsp.getIdValuePairsList()) {
        Log.d(TAG, "pair: id=" + info.getId() + ", value=" + info.getValue());
        Pair<Integer, Integer> id_val_pair =
            new Pair<>(info.getId(), info.getValue());
        dev_info_list.add(id_val_pair);
      }

      Message inner_msg = mHandler.obtainMessage(MSG_GET_DEV_INFO_LIST_COMPLETE, dev_info_list);
      inner_msg.sendToTarget();
    } else if (msg.getType() == Push.MsgType.SET_DEV_INFO_RSP && msg.hasSetDevInfoRsp()) {
      // SetDevInfoRsp rsp = msg.getSetDevInfoRsp();
      Log.d(TAG, "set_dev_info_rsp received, ignore it");
    } else {
      Log.e(TAG, "bad response");
    }
  }

  @Override
  public void surfaceCreated(SurfaceHolder holder) {
    Log.d(TAG, "surfaceCreated()");
  }

  @Override
  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    Log.d(TAG, "surfaceChanged()");
  }

  @Override
  public void surfaceDestroyed(SurfaceHolder holder) {
    Log.d(TAG, "surfaceDestroyed()");
  }
}
