package com.letmidi.monsys;

import java.util.LinkedList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.letmidi.monsys.account.MonsysHelper;
import com.letmidi.monsys.account.MonsysHelper.GetDevListCallback;

public class DevListActivity extends Activity implements OnItemClickListener, GetDevListCallback {
  private static final String TAG = "DevListActivity";

  private Button mRefreshButton;
  private ListView mListView;
  private final List<DevInfo> mDevList = new LinkedList<DevInfo>();
  private MyAdapter mListViewAdapter;
//  private final MyAsyncTask mQueryTask = new MyAsyncTask();

  private String mFgwId;

  private static final int MSG_GET_DEV_LIST_COMPLETE = 0x01;

  private final Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_GET_DEV_LIST_COMPLETE:
          mDevList.clear();
          List<DevInfo> dev_list = (List<DevInfo>) msg.obj;
          if (dev_list != null) {
            for (DevInfo info: dev_list) {
              mDevList.add(info);
            }
            mListViewAdapter.notifyDataSetChanged();
          }
          break;
      }
    }
  };

  public static class MyAdapter extends ArrayAdapter<DevInfo> {

    public MyAdapter(Context context, int resource, int textViewResourceId, List<DevInfo> objects) {
      super(context, resource, textViewResourceId, objects);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
      View view = super.getView(position, convertView, parent);

      DevInfo item = getItem(position);

      TextView dev_name_text = (TextView) view.findViewById(R.id.dev_name);
      dev_name_text.setText(item.name);

      TextView dev_addr_text = (TextView) view.findViewById(R.id.dev_addr);
      dev_addr_text.setText(String.valueOf(item.addr));

      return view;
    }
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_dev_list);

    Intent intent = getIntent();
    Bundle bundle = intent.getExtras();
    mFgwId = bundle.getString("fgw-id");
    if (mFgwId == null || mFgwId.length() <= 0) {
      AlertDialog.Builder builder = new AlertDialog.Builder(this);
      builder
          .setTitle("Bubu!")
          .setMessage("No fgw-id was supplied")
          .setPositiveButton("Fine", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              finish();
            }
          })
          .show();
    } else {
      mListViewAdapter = new MyAdapter(getApplicationContext(), R.layout.dev_item, R.id.dev_name, mDevList);
      mListView = (ListView) findViewById(R.id.dev_list);
      mListView.setAdapter(mListViewAdapter);
      mListView.setOnItemClickListener(this);

      mRefreshButton = (Button) findViewById(R.id.refresh_button);
      mRefreshButton.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
          mDevList.clear();
          mListViewAdapter.notifyDataSetChanged();
          getDevList();
        }
      });

      getDevList();
    }
  }

  private void getDevList() {
    MonsysHelper.getDevListAsync(mFgwId, this);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    Bundle bundle = new Bundle();
    bundle.putString("fgw-id", mFgwId);
    bundle.putInt("dev-addr", mDevList.get(position).addr);

    Intent intent = new Intent(getApplicationContext(), SmartLightActivity.class);
    intent.putExtras(bundle);

    startActivity(intent);
  }

  @Override
  public void onGetDevList(List<DevInfo> dev_list) {
    Log.d(TAG, "onGetDevList()");
    Message msg = mHandler.obtainMessage(MSG_GET_DEV_LIST_COMPLETE, dev_list);
    msg.sendToTarget();
  }
}
