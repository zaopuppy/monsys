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
import com.letsmidi.monsys.FgwInfo;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.protocol.MonsysServer;
import com.letsmidi.monsys.protocol.push.Push.ConnectRsp;
import com.letsmidi.monsys.protocol.push.Push.FGatewayInfo;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;

import java.util.LinkedList;
import java.util.List;

public class FgwListActivity extends MonsysActivity implements OnItemClickListener {

  private static final String TAG = "XXX";

  private Button mRefreshButton;
  private ListView mListView;
  //private String mAccount;
  private final List<FgwInfo> mFgwList = new LinkedList<>();
  private MyAdapter mListViewAdapter;

  private static final int MSG_GET_FGW_LIST_COMPLETE = 0x01;
  private static final int MSG_ENABLE_LIST_VIEW = 0x02;

  private final Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_GET_FGW_LIST_COMPLETE:
          mFgwList.clear();
          List<FgwInfo> fgw_list = (List<FgwInfo>) msg.obj;
          if (fgw_list != null) {
            for (FgwInfo info : fgw_list) {
              mFgwList.add(info);
            }
          }
          mListViewAdapter.notifyDataSetChanged();
          mRefreshButton.setEnabled(true);
          break;
        case MSG_ENABLE_LIST_VIEW:
          enableClick();
          break;
      }
    }
  };

  private static class MyAdapter extends ArrayAdapter<FgwInfo> {

    public MyAdapter(Context context, int resource, int textViewResourceId, List<FgwInfo> objects) {
      super(context, resource, textViewResourceId, objects);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
      View view = super.getView(position, convertView, parent);

      FgwInfo item = getItem(position);

      TextView fgw_id_text = (TextView) view.findViewById(R.id.fgw_id);
      fgw_id_text.setText(item.id);

      return view;
    }
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_fgw_list);

    mListViewAdapter = new MyAdapter(getApplicationContext(), R.layout.fgw_item, R.id.fgw_id, mFgwList);
    mListView = (ListView) findViewById(R.id.fgw_list);
    mListView.setAdapter(mListViewAdapter);
    mListView.setOnItemClickListener(this);

    mRefreshButton = (Button) findViewById(R.id.refresh_button);
    mRefreshButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        mFgwList.clear();
        mListViewAdapter.notifyDataSetChanged();
        // AccountManager.getFgwListAsync(mAccount,
        // FgwListActivity.this);
        getFgwList();
      }
    });

    getFgwList();
  }

  private void getFgwList() {
    mRefreshButton.setEnabled(false);
    MonsysServer.INSTANCE.getFgwList();
  }

  private void disableClick() {
    mListView.setEnabled(false);
    mListView.setClickable(false);
  }

  private void enableClick() {
    mListView.setEnabled(true);
    mListView.setClickable(true);
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    Log.d(TAG, "requestCode: " + requestCode + ", resultCode: " + resultCode);

    if (requestCode == 0x01 && resultCode == RESULT_OK) {
      // AccountManager
    } else {
      super.onActivityResult(requestCode, resultCode, data);
    }
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    disableClick();
    MonsysServer.INSTANCE.connectToFgw(mFgwList.get(position).id);
  }

  @Override
  public void onMonsysMessage(PushMsg msg) {
    switch (msg.getType()) {
      case GET_FGW_LIST_RSP:
        processFgwListRsp(msg);
        break;
      case CONNECT_RSP:
        processConnectRsp(msg);
        break;
      default:
        break;
    }
  }

  private void processConnectRsp(PushMsg msg) {
    if (!msg.hasConnectRsp()) {
      Log.e(TAG, "bad response: " + msg.getType());
      return;
    }

    mHandler.obtainMessage(MSG_ENABLE_LIST_VIEW).sendToTarget();

    ConnectRsp rsp = msg.getConnectRsp();

    if (rsp.getCode() != 0) {
      Log.e(TAG, "Failed to connect fgw");
      return;
    }

    // Bundle bundle = new Bundle();
    // bundle.putString("fgw-id", mFgwList.get(position).id);

    Intent intent = new Intent(getApplicationContext(), DevListActivity.class);
    // intent.putExtras(bundle);

    startActivity(intent);
  }

  private void processFgwListRsp(PushMsg msg) {
    if (msg.getType() != MsgType.GET_FGW_LIST_RSP || !msg.hasGetFgwListRsp()) {
      Log.e(TAG, "bad response: " + msg.getType());
      return;
    }

    List<FgwInfo> fgw_list = new LinkedList<>();
    FgwInfo fgw_info;
    for (FGatewayInfo info: msg.getGetFgwListRsp().getFgwInfosList()) {
      fgw_info = new FgwInfo();
      fgw_info.id = info.getId();
      fgw_info.name = info.getName();
      fgw_list.add(fgw_info);
    }

    Message inner_msg = mHandler.obtainMessage(MSG_GET_FGW_LIST_COMPLETE, fgw_list);
    inner_msg.sendToTarget();
  }
}
