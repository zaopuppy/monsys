package com.letmidi.monsys;

import java.util.LinkedList;
import java.util.List;

import com.letmidi.monsys.protocol.MonsysInterface;

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
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class FgwListActivity extends Activity implements OnItemClickListener {

  private ListView mListView;
  private String mAccount;
  private String mPassword;
  private final List<FgwInfo> mFgwList = new LinkedList<FgwInfo>();
  private MyAdapter mListViewAdapter;
  private final MyAsyncTask mLoginTask = new MyAsyncTask();
//  private static final Handler mHandler;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.activity_fgw_list);

    Intent intent = getIntent();
    Bundle bundle = intent.getExtras();
    mAccount = bundle.getString("account");
    mPassword = bundle.getString("password");
    if (mAccount == null || mAccount.length() <= 0 ||
        mPassword == null || mPassword.length() <= 0) {
      AlertDialog.Builder builder = new AlertDialog.Builder(this);
      builder
          .setTitle("Bubu!")
          .setMessage("No account or password was supplied")
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

      mLoginTask.execute(mAccount, mPassword);
    }
  }

  @Override
  protected void onDestroy() {
    if (!mLoginTask.isCancelled()) {
      mLoginTask.cancel(true);
    }
    super.onDestroy();
  }

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

  private class MyAsyncTask extends AsyncTask<String, Integer, List<FgwInfo>> {

    @Override
    protected List<FgwInfo> doInBackground(String... params) {

      String account = params[0];
      String password = params[1];

      if (!MonsysInterface.login(account, password)) {
        Log.e("XXX", "Failed to login monsys with ([" + account + "], [" + password + "])");
        return null;
      }

      List<FgwInfo> fgw_list = MonsysInterface.getFgwList(account);
      if (fgw_list == null) {
        Log.e("XXX", "Failed to get fgw list");
        return null;
      }

      return fgw_list;
    }

    @Override
    protected void onPostExecute(List<FgwInfo> result) {
      if (result == null) {
        Log.e("XXX", "task failed");
        Toast.makeText(getApplicationContext(), "Failed to get fgw list:(", Toast.LENGTH_SHORT).show();
        return;
      }

      Log.i("XXX", "got " + result.size() + " fgws");

      mFgwList.clear();
      for (FgwInfo info : result) {
        Log.i("XXX", "fgw: " + info);
        mFgwList.add(info);
      }

      mListViewAdapter.notifyDataSetChanged();
    }

  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
//    Toast.makeText(this, "clicked: " + position, Toast.LENGTH_SHORT).show();

    Bundle bundle = new Bundle();
    bundle.putString("fgw-id", mFgwList.get(position).id);

    Intent intent = new Intent(getApplicationContext(), DevListActivity.class);
    intent.putExtras(bundle);

    startActivity(intent);
  }
}
