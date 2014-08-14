package com.letsmidi.monsys.ui;

import java.util.LinkedList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
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
import com.letsmidi.monsys.protocol.MonsysServer.Listener;
import com.letsmidi.monsys.protocol.Push.GetDevInfoRsp;
import com.letsmidi.monsys.protocol.Push.IdValuePair;
import com.letsmidi.monsys.protocol.Push.MsgType;
import com.letsmidi.monsys.protocol.Push.PushMsg;

public class SmartLightActivity extends Activity implements OnSeekBarChangeListener, Listener {

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

    private Button mSwitchButton;

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
        Toast.makeText(getApplicationContext(), "dev info got successfully", Toast.LENGTH_SHORT).show();
    }

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
        } else {
            mRefreshButton = (Button) findViewById(R.id.refresh_button);
            mRefreshButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    List<Integer> id_list = new LinkedList<Integer>();
                    id_list.add(0);
                    // MonsysHelper.getDevInfoListAsync(mFgwId, mDevAddr,
                    // id_list, SmartLightActivity.this);
                    MonsysServer.INSTANCE.getDevInfo(mDevAddr, id_list);
                }
            });

            mColorPickView = (SurfaceView) findViewById(R.id.color_pick_view);
            mColorPickViewHolder = mColorPickView.getHolder();

            mRSeekBar = (SeekBar) findViewById(R.id.r_slider);
            mGSeekBar = (SeekBar) findViewById(R.id.g_slider);
            mBSeekBar = (SeekBar) findViewById(R.id.b_slider);

            mSwitchButton = (Button) findViewById(R.id.switch_button);
            mSwitchButton.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    List<Pair<Integer, Integer>> id_vals = new LinkedList<Pair<Integer, Integer>>();
                    id_vals.add(new Pair<Integer, Integer>(ID_COLOR_R, 0));
                    id_vals.add(new Pair<Integer, Integer>(ID_COLOR_B, 0));
                    id_vals.add(new Pair<Integer, Integer>(ID_COLOR_G, 0));
                    // MonsysHelper.setDevInfoListAsync(mFgwId, mDevAddr,
                    // id_vals, SmartLightActivity.this);
                    MonsysServer.INSTANCE.setDevInfo(mDevAddr, id_vals);
                }
            });

            List<Integer> id_list = new LinkedList<Integer>();
            id_list.add(0);
            // MonsysHelper.getDevInfoListAsync(mFgwId, mDevAddr, id_list,
            // this);
            MonsysServer.INSTANCE.getDevInfo(mDevAddr, id_list);
        }

        //
        initUIComponents();
    }

    private void initUIComponents() {
        // mRSeekBar.setEnabled(false);
        // mGSeekBar.setEnabled(false);
        // mBSeekBar.setEnabled(false);

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

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        Log.d(TAG, "onStartTrackingTouch(" + seekBar.getProgress() + ")");
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        Log.d(TAG, "onStopTrackingTouch(" + seekBar.getProgress() + ")");

        List<Pair<Integer, Integer>> id_vals = new LinkedList<Pair<Integer, Integer>>();

        if (seekBar == mRSeekBar) {
            id_vals.add(new Pair<Integer, Integer>(ID_COLOR_R, seekBar.getProgress()));
        } else if (seekBar == mGSeekBar) {
            id_vals.add(new Pair<Integer, Integer>(ID_COLOR_G, seekBar.getProgress()));
        } else if (seekBar == mBSeekBar) {
            id_vals.add(new Pair<Integer, Integer>(ID_COLOR_B, seekBar.getProgress()));
        } else {
            Log.e(TAG, "unknown seekbar");
            return;
        }

        // MonsysHelper.setDevInfoListAsync(mFgwId, mDevAddr, id_vals, this);
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
    public void onConnected() {
        // TODO Auto-generated method stub

    }

    @Override
    public void onDisconnected() {
        // TODO Auto-generated method stub

    }

    @Override
    public void onLoggedIn() {
        // TODO Auto-generated method stub

    }

    @Override
    public void onMessage(PushMsg msg) {
        if (msg.getType() == MsgType.GET_DEV_INFO_RSP && msg.hasGetDevInfoRsp()) {
            Log.d(TAG, "get_dev_info_rsp received");
            GetDevInfoRsp rsp = msg.getGetDevInfoRsp();
            List<Pair<Integer, Integer>> dev_info_list = new LinkedList<Pair<Integer,Integer>>();

            for (IdValuePair info: rsp.getIdValuePairsList()) {
                Log.d(TAG, "pair: id=" + info.getId() + ", value=" + info.getValue());
                Pair<Integer, Integer> id_val_pair =
                        new Pair<Integer, Integer>(info.getId(), info.getValue());
                dev_info_list.add(id_val_pair);
            }

            Message inner_msg = mHandler.obtainMessage(MSG_GET_DEV_INFO_LIST_COMPLETE, dev_info_list);
            inner_msg.sendToTarget();
        } else if (msg.getType() == MsgType.SET_DEV_INFO_RSP && msg.hasSetDevInfoRsp()) {
            // SetDevInfoRsp rsp = msg.getSetDevInfoRsp();
            Log.d(TAG, "set_dev_info_rsp received, ignore it");
        } else {
            Log.e(TAG, "bad response");
        }
    }

    @Override
    public void onException(Throwable cause) {
        // TODO Auto-generated method stub

    }
}
