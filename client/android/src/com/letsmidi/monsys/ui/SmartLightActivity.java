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
import com.letsmidi.monsys.sdk.MonsysClient;
import com.letsmidi.monsys.sdk.MonsysConnection;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.GenericFutureListener;

import java.util.ArrayList;
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

    private String mFgwId;
    private int mDevAddr;

    private static final int MSG_GET_DEV_INFO_LIST_COMPLETE = 0x01;

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
                getAllDevInfo();
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
                setPreview(0, 0, 0);

                MonsysConnection.DevInfo dev_info = new MonsysConnection.DevInfo();
                dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_R, 0));
                dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_G, 0));
                dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_B, 0));
                setDevInfo(dev_info);
            }
        });

        getAllDevInfo();

        mRSeekBar.setOnSeekBarChangeListener(this);
        mGSeekBar.setOnSeekBarChangeListener(this);
        mBSeekBar.setOnSeekBarChangeListener(this);
    }

    private void setDevInfo(MonsysConnection.DevInfo dev_info) {
        MonsysClient.getConnection().setDevInfo(mFgwId, mDevAddr, dev_info).addListener(
            new GenericFutureListener<Future<Integer>>() {
                @Override
                public void operationComplete(Future<Integer> future) throws Exception {
                    if (future == null || !future.isSuccess()) {
                        Log.e(TAG, "failed to set device info");
                        Toast.makeText(getApplicationContext(), "failed to set dev info", Toast.LENGTH_SHORT).show();
                        return;
                    }

                    int code = future.get();
                    if (code == 0) {
                        Toast.makeText(getApplicationContext(), "dev info set", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(getApplicationContext(), "failed to set dev info: " + code, Toast.LENGTH_SHORT).show();
                    }
                }
            }
        );
    }

    private void getAllDevInfo() {
        final ArrayList<Integer> id_list = new ArrayList<>();
        id_list.add(0);

        MonsysClient.getConnection().getDevInfo(mFgwId, mDevAddr, id_list).addListener(
            new GenericFutureListener<Future<MonsysConnection.DevInfo>>() {
                @Override
                public void operationComplete(Future<MonsysConnection.DevInfo> future) throws Exception {
                    if (future == null || !future.isSuccess()) {
                        Toast.makeText(getApplicationContext(), "failed to get dev info", Toast.LENGTH_SHORT).show();
                        return;
                    }

                    MonsysConnection.DevInfo dev_info = future.get();

                    for (MonsysConnection.IdValue id_value : dev_info.idValueList) {
                        if (id_value.id == ID_COLOR_R) { // R
                            Log.d(TAG, "Enable R-SeekBar: " + id_value.value);
                            mRSeekBar.setEnabled(true);
                            mRSeekBar.setProgress(mapColorValue(id_value.value));
                        } else if (id_value.id == ID_COLOR_G) {
                            Log.d(TAG, "Enable G-SeekBar: " + id_value.value);
                            mGSeekBar.setEnabled(true);
                            mGSeekBar.setProgress(mapColorValue(id_value.value));
                        } else if (id_value.id == ID_COLOR_B) {
                            Log.d(TAG, "Enable B-SeekBar: " + id_value.value);
                            mBSeekBar.setEnabled(true);
                            mBSeekBar.setProgress(mapColorValue(id_value.value));
                        } else {
                            Log.e(TAG, "Unknown id: " + id_value.id + "=" + id_value.value);
                        }
                    }

                    setPreview(mRSeekBar.getProgress(), mGSeekBar.getProgress(), mBSeekBar.getProgress());

                    Toast.makeText(getApplicationContext(), "dev info got successfully", Toast.LENGTH_SHORT).show();
                }
            }
        );
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

        MonsysConnection.DevInfo dev_info = new MonsysConnection.DevInfo();

        if (seekBar == mRSeekBar) {
            dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_R, seekBar.getProgress()));
        } else if (seekBar == mGSeekBar) {
            dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_G, seekBar.getProgress()));
        } else if (seekBar == mBSeekBar) {
            dev_info.idValueList.add(new MonsysConnection.IdValue(ID_COLOR_B, seekBar.getProgress()));
        } else {
            Log.e(TAG, "unknown seekbar");
            return;
        }

        setDevInfo(dev_info);
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
