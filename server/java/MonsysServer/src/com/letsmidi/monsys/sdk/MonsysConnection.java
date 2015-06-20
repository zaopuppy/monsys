package com.letsmidi.monsys.sdk;

import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.util.BaseClientConnection;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.util.concurrent.DefaultPromise;
import io.netty.util.concurrent.Future;

import java.util.ArrayList;

/**
 * Created by zero on 6/11/15.
 */
public class MonsysConnection extends BaseClientConnection<Push.PushMsg> {
    private static final String TAG = "MonsysConnection";

    private static void log(String msg) {
        System.out.println(msg);
    }

    public enum State {
        DISCONNECTED,
        CONNECTED,
        LOGGED_IN,
    }

    private final String mHost;
    private final int mPort;
    private final SequenceGenerator mIdGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private State mState = State.DISCONNECTED;

    public MonsysConnection(String host, int port, NioEventLoopGroup group) {
        super(group);
        mHost = host;
        mPort = port;
    }

    public State getState() {
        return mState;
    }

    private void setState(State state) {
        this.mState = state;
    }

    public boolean isLoggedIn() {
        return mState == State.LOGGED_IN;
    }

    private SimpleChannelInboundHandler<Push.PushMsg> mHandler = new SimpleChannelInboundHandler<Push.PushMsg>() {
        @Override
        protected void channelRead0(ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
            log("received: " + msg.getType());
            onResponse(msg);
        }

        @Override
        public void channelActive(ChannelHandlerContext ctx) throws Exception {
            super.channelActive(ctx);
            setState(State.CONNECTED);
        }

        @Override
        public void channelInactive(ChannelHandlerContext ctx) throws Exception {
            super.channelInactive(ctx);
            setState(State.DISCONNECTED);
        }
    };

    private void innerLogin(String account, String password, DefaultPromise<Integer> promise) {
        Push.PushMsg.Builder builder =
            MsgUtil.newPushMsgBuilder(Push.MsgType.ADMIN_CLIENT_LOGIN, mIdGenerator.next());

        Push.AdminClientLogin.Builder login = Push.AdminClientLogin.newBuilder();
        login.setAccount(account);
        login.setPassword(password);

        builder.setAdminClientLogin(login);

        writeAndFlush(builder.build(), msg -> {
            if (msg == null || !msg.hasAdminClientLoginRsp()) {
                log("bad response");
                promise.setFailure(new Throwable());
                return;
            }

            Push.AdminClientLoginRsp rsp = msg.getAdminClientLoginRsp();
            if (rsp.getCode() == 0) {
                setState(State.LOGGED_IN);
            }
            promise.setSuccess(rsp.getCode());
        });
    }

    public Future<Integer> login(String account, String password) {
        final DefaultPromise<Integer> promise;

        if (channel() == null || channel().isActive()) {
            ChannelFuture future = connect(mHost, mPort);
            promise = new DefaultPromise<>(future.channel().eventLoop());
            future.addListener(f -> innerLogin(account, password, promise));

            return promise;
        } else {
            promise = new DefaultPromise<>(channel().eventLoop());
            innerLogin(account, password, promise);

            return promise;
        }
    }

    public static class Fgw {
        public final String id;
        public final String name;
        public final String desc;

        public Fgw(String id, String name, String desc) {
            this.id = id;
            this.name = name;
            this.desc = desc;
        }
    }

    public Future<ArrayList<Fgw>> getFgwList() {
        DefaultPromise<ArrayList<Fgw>> promise = new DefaultPromise<>(channel().eventLoop());

        Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_FGW_LIST, mIdGenerator.next());
        builder.setGetFgwList(Push.GetFgwList.newBuilder());

        writeAndFlush(builder.build(), msg -> {
            if (msg == null || !msg.hasGetFgwListRsp()) {
                log("bad response");
                promise.setFailure(new Throwable());
                return;
            }

            Push.GetFgwListRsp get_fgw_list_rsp = msg.getGetFgwListRsp();
            log("response code: " + get_fgw_list_rsp.getCode());
            log("response code: " + get_fgw_list_rsp.getFgwInfosCount());

            if (get_fgw_list_rsp.getCode() != 0) {
                promise.setSuccess(null);
                return;
            }

            ArrayList<Fgw> fgw_list = new ArrayList<>();
            for (Push.FGatewayInfo info : get_fgw_list_rsp.getFgwInfosList()) {
                fgw_list.add(new Fgw(info.getId(), info.getName(), info.getDesc()));
            }

            promise.setSuccess(fgw_list);
        });

        return promise;
    }

    public static class IdValue {
        public final int id;
        public final int value;

        public IdValue(int id, int value) {
            this.id = id;
            this.value = value;
        }
    }

    public static class DevInfo {
        public ArrayList<IdValue> idValueList = new ArrayList<>();
    }

    public Future<DevInfo> getDevInfo(String fgwId, int addr, ArrayList<Integer> id_list) {
        DefaultPromise<DevInfo> promise = new DefaultPromise<>(channel().eventLoop());

        Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_DEV_INFO, mIdGenerator.next());

        Push.GetDevInfo.Builder get_dev_info = Push.GetDevInfo.newBuilder();
        get_dev_info.setDeviceId(fgwId);
        get_dev_info.setAddr(addr);

        for (Integer id: id_list) {
            get_dev_info.addItemIds(id);
        }

        builder.setGetDevInfo(get_dev_info);

        writeAndFlush(builder.build(), msg -> {
            if (msg == null || !msg.hasGetDevInfoRsp()) {
                log("bad response");
                promise.setFailure(new Throwable());
                return;
            }

            Push.GetDevInfoRsp get_dev_info_rsp = msg.getGetDevInfoRsp();
            log("response code: " + get_dev_info_rsp.getCode());
            log("response size: " + get_dev_info_rsp.getIdValuePairsCount());

            if (get_dev_info_rsp.getCode() != 0) {
                promise.setSuccess(null);
                return;
            }

            DevInfo dev_info = new DevInfo();
            for (Push.IdValuePair pair : get_dev_info_rsp.getIdValuePairsList()) {
                log("id=" + pair.getId() + ", value=" + pair.getValue());
                dev_info.idValueList.add(new IdValue(pair.getId(), pair.getValue()));
            }

            promise.setSuccess(dev_info);
        });

        return promise;
    }

    public Future<Integer> setDevInfo(String fgw_id, int addr, DevInfo dev_info) {
        DefaultPromise<Integer> promise = new DefaultPromise<>(channel().eventLoop());

        Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.SET_DEV_INFO, mIdGenerator.next());

        Push.SetDevInfo.Builder set_dev_info = Push.SetDevInfo.newBuilder();
        set_dev_info.setAddr(addr);
        set_dev_info.setDeviceId(fgw_id);

        Push.IdValuePair.Builder id_value_builder = Push.IdValuePair.newBuilder();
        for (IdValue id_value: dev_info.idValueList) {
            id_value_builder.setId(id_value.id);
            id_value_builder.setValue(id_value.value);
            set_dev_info.addIdValuePairs(id_value_builder.build());
        }

        builder.setSetDevInfo(set_dev_info);

        writeAndFlush(builder.build(), msg -> {
            if (msg == null || !msg.hasSetDevInfoRsp()) {
                log("bad response");
                promise.setFailure(new Throwable());
                return;
            }

            Push.SetDevInfoRsp info_rsp = msg.getSetDevInfoRsp();
            log("response code: " + info_rsp.getCode());

            promise.setSuccess(info_rsp.getCode());
        });

        return promise;
    }

    public static class Dev {
        public final String name;
        public final int type;
        public final int addr;

        public Dev(String name, int type, int addr) {
            this.name = name;
            this.type = type;
            this.addr = addr;
        }
    }

    public Future<ArrayList<Dev>> getDevList(String fgwId) {
        DefaultPromise<ArrayList<Dev>> promise = new DefaultPromise<>(channel().eventLoop());

        Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_DEV_LIST, mIdGenerator.next());

        Push.GetDevList.Builder get_dev_list = Push.GetDevList.newBuilder();
        get_dev_list.setDeviceId(fgwId);

        builder.setGetDevList(get_dev_list);

        writeAndFlush(builder.build(), msg -> {
            if (msg == null || !msg.hasGetDevListRsp()) {
                log("bad response");
                promise.setFailure(new Throwable());
                return;
            }

            Push.GetDevListRsp get_dev_list_rsp = msg.getGetDevListRsp();
            log("response code: " + get_dev_list_rsp.getCode());
            log("response size: " + get_dev_list_rsp.getDevInfosList().size());
            if (get_dev_list_rsp.getCode() != 0) {
                promise.setSuccess(null);
                return;
            }

            ArrayList<Dev> dev_list = new ArrayList<>();
            for (Push.DeviceInfo info: get_dev_list_rsp.getDevInfosList()) {
                log("name=" + info.getName() + ", type=" + info.getType() + ", addr=" + info.getAddr());
                dev_list.add(new Dev(info.getName(), info.getType(), info.getAddr()));
            }

            promise.setSuccess(dev_list);
        });

        return promise;
    }

    @Override
    protected void setChannel(Channel channel) {
        super.setChannel(channel);
        if (channel != null) {
            ChannelPipeline pipeline = channel.pipeline();
            pipeline.addLast("header-prepender", new ProtobufVarint32LengthFieldPrepender());
            pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
            pipeline.addLast("encoder", new ProtobufEncoder());
            pipeline.addLast("decoder", new ProtobufDecoder(
                Push.PushMsg.getDefaultInstance()));
            pipeline.addLast("handler", mHandler);
        }
    }

    @Override
    public Channel popChannel() {
        Channel channel = channel();
        if (channel == null) {
            return null;
        }

        ChannelPipeline pipeline = channel.pipeline();

        pipeline.remove("header-prepender");
        pipeline.remove("frame-decoder");
        pipeline.remove("encoder");
        pipeline.remove("decoder");
        pipeline.remove("handler");

        return channel;
    }

    @Override
    protected boolean saveRoute(Push.PushMsg msg, Callback<Push.PushMsg> callback) {
        getRouteMap().put(msg.getSequence(), new RouteItem<>(msg.getSequence(), callback));
        return true;
    }

    @Override
    protected RouteItem<Push.PushMsg> removeRoute(Push.PushMsg msg) {
        return getRouteMap().remove(msg.getSequence());
    }

}
