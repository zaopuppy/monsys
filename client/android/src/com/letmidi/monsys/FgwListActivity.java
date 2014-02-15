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

import com.letmidi.monsys.account.AccountManager;
import com.letmidi.monsys.account.AccountManager.GetFgwListCallback;

public class FgwListActivity extends Activity implements OnItemClickListener, GetFgwListCallback {

  private static final String TAG = "XXX";

  private Button mRefreshButton;
  private ListView mListView;
  private String mAccount;
//  private String mPassword;
  private final List<FgwInfo> mFgwList = new LinkedList<FgwInfo>();
  private MyAdapter mListViewAdapter;
//  private MyAsyncTask mLoginTask = new MyAsyncTask();

  private static final int MSG_GET_FGW_LIST_COMPLETE = 0x01;

  private final Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      switch (msg.what) {
        case MSG_GET_FGW_LIST_COMPLETE:
          mFgwList.clear();
          List<FgwInfo> fgw_list = (List<FgwInfo>) msg.obj;
          if (fgw_list != null) {
            for (FgwInfo info: fgw_list) {
              mFgwList.add(info);
            }
          }
          mListViewAdapter.notifyDataSetChanged();
          mRefreshButton.setEnabled(true);
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

    Intent intent = getIntent();
    Bundle bundle = intent.getExtras();
    mAccount = bundle.getString("account");
    if (mAccount == null || mAccount.length() <= 0) {
      AlertDialog.Builder builder = new AlertDialog.Builder(this);
      builder
          .setTitle("Bubu!")
          .setMessage("No account was supplied")
          .setPositiveButton("Fine", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              finish();
            }
          })
          .show();
    } else {
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
//          AccountManager.getFgwListAsync(mAccount, FgwListActivity.this);
          getFgwList();
        }
      });

//      mLoginTask.execute(mAccount);
//      AccountManager.getFgwListAsync(mAccount, this);
      getFgwList();

//      Intent loginIntent = new Intent(getApplicationContext(), LoginActivity.class);
//      startActivityForResult(loginIntent, 0x01);
    }
  }

  private void getFgwList() {
    mRefreshButton.setEnabled(false);
    AccountManager.getFgwListAsync(mAccount, this);
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    Log.d(TAG, "requestCode: " + requestCode + ", resultCode: " + resultCode);

    if (requestCode == 0x01 && resultCode == RESULT_OK) {
//      AccountManager
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
    Bundle bundle = new Bundle();
    bundle.putString("fgw-id", mFgwList.get(position).id);

    Intent intent = new Intent(getApplicationContext(), DevListActivity.class);
    intent.putExtras(bundle);

    startActivity(intent);
  }

  @Override
  public void onGetFgwList(List<FgwInfo> fgw_list) {
    Log.d(TAG, "onGetFGWList()");
    Message msg = mHandler.obtainMessage(MSG_GET_FGW_LIST_COMPLETE, fgw_list);
    msg.sendToTarget();
  }
}
