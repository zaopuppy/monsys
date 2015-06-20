package com.letsmidi.monsys.ui;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import android.widget.AdapterView.OnItemClickListener;
import com.letsmidi.monsys.DevInfo;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.sdk.MonsysClient;
import com.letsmidi.monsys.sdk.MonsysConnection;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.GenericFutureListener;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class DevListActivity extends MonsysActivity implements OnItemClickListener {
    private static final String TAG = "DevListActivity";

    private final List<DevInfo> mDevList = new LinkedList<DevInfo>();
    private MyAdapter mListViewAdapter;

    private String mFgwId;

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
        ListView listView = (ListView) findViewById(R.id.dev_list);
        listView.setAdapter(mListViewAdapter);
        listView.setOnItemClickListener(this);

        Button refreshButton = (Button) findViewById(R.id.refresh_button);
        refreshButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDevList.clear();
                mListViewAdapter.notifyDataSetChanged();
                getDevList();
            }
        });

        getDevList();
    }

    private void getDevList() {
        MonsysClient.getConnection().getDevList(mFgwId).addListener(
            new GenericFutureListener<Future<ArrayList<MonsysConnection.Dev>>>() {
                @Override
                public void operationComplete(Future<ArrayList<MonsysConnection.Dev>> future) throws Exception {
                    if (future == null || !future.isSuccess()) {
                        Log.e(TAG, "bad response");
                        return;
                    }

                    mDevList.clear();
                    ArrayList<MonsysConnection.Dev> dev_list = future.get();
                    for (MonsysConnection.Dev dev: dev_list) {
                        mDevList.add(new DevInfo(dev.name, dev.addr, dev.type));
                    }
                    mListViewAdapter.notifyDataSetChanged();
                }
            });
    }

    @Override
    protected void onResume() {
        //if (!MonsysServer.INSTANCE.isLoggedIn()) {
        //  finish();
        //}
        super.onResume();
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
}

