package com.letsmidi.monsys.ui;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import android.widget.AdapterView.OnItemClickListener;
import com.letsmidi.monsys.FgwInfo;
import com.letsmidi.monsys.R;
import com.letsmidi.monsys.sdk.MonsysClient;
import com.letsmidi.monsys.sdk.MonsysConnection;
import io.netty.util.concurrent.Future;
import io.netty.util.concurrent.GenericFutureListener;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class FgwListActivity extends MonsysActivity implements OnItemClickListener {

    private static final String TAG = "XXX";

    private Button mRefreshButton;
    private ListView mListView;
    private final List<FgwInfo> mFgwList = new LinkedList<>();
    private MyAdapter mListViewAdapter;

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
                refreshFgwList();
            }
        });

        refreshFgwList();
    }

    private void refreshFgwList() {
        mRefreshButton.setEnabled(false);
        mFgwList.clear();
        mListViewAdapter.notifyDataSetChanged();
        MonsysClient.getConnection().getFgwList().addListener(
            new GenericFutureListener<Future<ArrayList<MonsysConnection.Fgw>>>() {
                @Override
                public void operationComplete(Future<ArrayList<MonsysConnection.Fgw>> future) throws Exception {
                    try {
                        if (future == null || !future.isSuccess()) {
                            Log.d(TAG, "failed to get fgw_list");
                            return;
                        }

                        final ArrayList<MonsysConnection.Fgw> fgw_list = future.get();
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mFgwList.clear();
                                for (MonsysConnection.Fgw fgw: fgw_list) {
                                    mFgwList.add(new FgwInfo(fgw.id, fgw.name));
                                }
                                mListViewAdapter.notifyDataSetChanged();
                            }
                        });
                    } finally {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mRefreshButton.setEnabled(true);
                            }
                        });
                    }
                }
            });
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
        Bundle bundle = new Bundle();
        bundle.putString("fgw-id", mFgwList.get(position).id);
        Intent intent = new Intent(getApplicationContext(), DevListActivity.class);
        intent.putExtras(bundle);
        startActivity(intent);
    }
}

