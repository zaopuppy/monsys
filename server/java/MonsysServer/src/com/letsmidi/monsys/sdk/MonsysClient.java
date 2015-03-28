package com.letsmidi.monsys.sdk;


import java.util.ArrayList;

import com.letsmidi.monsys.protocol.client.Client;
import io.netty.channel.ChannelFuture;
import io.netty.channel.nio.NioEventLoopGroup;


// TODO: timeout
public class MonsysClient extends BaseClient<Client.ClientMsg> {

    private enum State {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
    }

    private final NioEventLoopGroup mWorker;
    private final MonsysCallback mCallback;

    private State mState = State.DISCONNECTED;

    public MonsysClient(NioEventLoopGroup worker, MonsysCallback callback) {
        mWorker = worker;
        mCallback = callback;
    }

    public void queryFgwList(Callback<ArrayList<String>> callback) {
        Client.ClientMsg.Builder builder = Client.ClientMsg.newBuilder();
        send(builder.build(), result -> {
            if (result.getType() != Client.MsgType.GET_FGW_LIST_RSP) {
                callback.onResponse(null);
                return;
            }

            ArrayList<String> fgw_list = new ArrayList<>();
            callback.onResponse(fgw_list);
        });
    }

    @Override
    public void connect() {
        if (getState() != State.DISCONNECTED) {
            return;
        }

        setState(State.CONNECTING);

        MonsysClientConnection connection = new MonsysClientConnection(mWorker);
        ChannelFuture login_connect_future = connection.connect("127.0.0.1", 1988);
        login_connect_future.addListener((ChannelFuture future) -> {
            if (!future.isSuccess()) {
                setState(State.DISCONNECTED);
                mCallback.onDisconnected();
                return;
            }

            login(login_rsp -> {
                connect();
            });
        });
    }

    private void login(Callback<Object> callback) {
        //
    }


    private State getState() {
        return mState;
    }

    private void setState(State newState) {
        mState = newState;
    }

    @Override
    public boolean isConnected() {
        return mState == State.DISCONNECTED;
    }

    @Override
    protected void saveRoute(Client.ClientMsg msg, Callback<Client.ClientMsg> callback) {
        int sequence = msg.getSequence();
        if (getRouteMap().containsKey(sequence)) {
            // ERROR
            return;
        }

        getRouteMap().put(msg.getSequence(), new RouteItem<>(sequence, callback));
    }

    @Override
    protected RouteItem<Client.ClientMsg> findRoute(Client.ClientMsg msg) {
        return getRouteMap().getOrDefault(msg.getSequence(), null);
    }
}
