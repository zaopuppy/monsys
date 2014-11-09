package com.letsmidi.monsys.ui;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import android.widget.AdapterView.OnItemClickListener;
import com.letsmidi.monsys.DevInfo;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;
import com.letsmidi.monsys.protocol.push.Push.DeviceInfo;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;

import java.util.LinkedList;
import java.util.List;

public class DevListActivity extends MonsysActivity implements OnItemClickListener {
  private static final String TAG = "DevListActivity";

  private Button mRefreshButton;
  private ListView mListView;
  private final List<DevInfo> mDevList = new LinkedList<DevInfo>();
  private MyAdapter mListViewAdapter;

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
            for (DevInfo info : dev_list) {
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
    //        }
  }

  @Override
  protected void onResume() {
    //if (!MonsysServer.INSTANCE.isLoggedIn()) {
    //  finish();
    //}
    super.onResume();
  }

  private void getDevList() {
    MonsysServer.INSTANCE.getDevList();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    Bundle bundle = new Bundle();
    // bundle.putString("fgw-id", mFgwId);
    bundle.putInt("dev-addr", mDevList.get(position).addr);

    Intent intent = new Intent(getApplicationContext(), SmartLightActivity.class);
    intent.putExtras(bundle);

    startActivity(intent);
  }

  @Override
  public void onMonsysMessage(PushMsg msg) {
    if (msg.getType() != MsgType.GET_DEV_LIST_RSP || !msg.hasGetDevListRsp()) {
      Log.e(TAG, "bad response: " + msg.getType());
      return;
    }

    List<DevInfo> dev_list = new LinkedList<DevInfo>();
    DevInfo dev_info;
    for (DeviceInfo info: msg.getGetDevListRsp().getDevInfosList()) {
      dev_info = new DevInfo();
      dev_info.addr = info.getAddr();
      dev_info.name = info.getName();
      dev_info.type = info.getType();
      dev_list.add(dev_info);
    }

    Message inner_msg = mHandler.obtainMessage(MSG_GET_DEV_LIST_COMPLETE, dev_list);
    inner_msg.sendToTarget();
  }
}




