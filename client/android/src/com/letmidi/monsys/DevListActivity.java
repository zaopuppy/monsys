package com.letmidi.monsys;

import java.util.LinkedList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.letmidi.monsys.protocol.MonsysInterface;

public class DevListActivity extends Activity implements OnItemClickListener {
  private static final String TAG = "DevListActivity";

  private Button mRefreshButton;
  private ListView mListView;
  private final List<DevInfo> mDevList = new LinkedList<DevInfo>();
  private MyAdapter mListViewAdapter;
  private MyAsyncTask mQueryTask = new MyAsyncTask();

  private String mFgwId;

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
          if (mQueryTask != null) {
            mQueryTask.cancel(true);
          }
          mQueryTask = new MyAsyncTask();
          mQueryTask.execute(mFgwId);
        }
      });

      mQueryTask.execute(mFgwId);
    }
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
  }

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

  private class MyAsyncTask extends AsyncTask<String, Integer, List<DevInfo>> {

    @Override
    protected List<DevInfo> doInBackground(String... params) {

      String fgw_id = params[0];

      List<DevInfo> dev_list = MonsysInterface.getDevList(fgw_id);
      if (dev_list == null) {
        Log.e("XXX", "Failed to get dev list");
        return null;
      }

      return dev_list;
    }

    @Override
    protected void onPostExecute(List<DevInfo> result) {
      if (result == null) {
        Log.e("XXX", "task failed");
        Toast.makeText(getApplicationContext(), "Failed to get dev list:(", Toast.LENGTH_SHORT).show();
        return;
      }

      Log.i("XXX", "got " + result.size() + " devs");
      Toast.makeText(getApplicationContext(), "dev list got successfully", Toast.LENGTH_SHORT).show();

      mDevList.clear();
      for (DevInfo info : result) {
        Log.i("XXX", "fgw: " + info);
        mDevList.add(info);
      }

      mListViewAdapter.notifyDataSetChanged();
    }

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
}
