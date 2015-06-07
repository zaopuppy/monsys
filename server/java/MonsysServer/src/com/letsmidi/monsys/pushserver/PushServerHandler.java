package com.letsmidi.monsys.pushserver;

import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;

public class PushServerHandler extends SimpleChannelInboundHandler<PushMsg> {
    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

    private enum STATE {
        WAITING_FOR_LOGIN,
        LOGGED_IN,
    }

    private STATE mState = STATE.WAITING_FOR_LOGIN;

    public PushServerHandler(HashedWheelTimer timer) {
        //mSessionManager = new SessionManager<>(timer);
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
        mLogger.info("current state: " + getState().name());
        switch (getState()) {
            case WAITING_FOR_LOGIN: {
                onWaitingForLogin(ctx, msg);
                break;
            }
            case LOGGED_IN: {
                onLoggedIn(ctx, msg);
                break;
            }
            default:
                break;
        }
    }

    private void onWaitingForLogin(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
        mLogger.info("onWaitingForLogin: " + msg.getType());
        if (!msg.getType().equals(MsgType.PUSH_CLIENT_LOGIN) || !msg.hasPushClientLogin()) {
            mLogger.severe("Loggin first please");
            ctx.close();
            return;
        }

        Push.PushClientLogin login = msg.getPushClientLogin();

        InMemInfo.INSTANCE.getPushClientMap().put(
                login.getDeviceId(), new PushClient(login.getDeviceId(), ctx.channel()));
        ctx.channel().closeFuture().addListener(
                future -> InMemInfo.INSTANCE.getPushClientMap().remove(login.getDeviceId()));

        setState(STATE.LOGGED_IN);

        mLogger.info("Logged in successfully: " + login.getDeviceId());

        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.PUSH_CLIENT_LOGIN_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        Push.PushClientLoginRsp.Builder login_rsp = Push.PushClientLoginRsp.newBuilder();
        login_rsp.setCode(0);

        builder.setPushClientLoginRsp(login_rsp);

        ctx.writeAndFlush(builder.build());
    }

    private void onLoggedIn(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
        mLogger.info("onLoggedIn: " + msg.getType());
        switch (msg.getType()) {
            case HEARTBEAT:
                // TODO
                break;
            case BIND_RSP:
                break;
            case CONNECT_RSP:
                break;
            case GET_DEV_INFO_RSP:
                handleGetDevInfoRsp(ctx, msg);
                break;
            case GET_DEV_LIST_RSP:
                handleGetDevListRsp(ctx, msg);
                break;
            case SET_DEV_INFO_RSP:
                handleSetDevInfoRsp(ctx, msg);
                break;
            default:
                break;
        }
    }

    private void handleRsp(ChannelHandlerContext ctx, PushMsg msg) {
        RouteRecord record = InMemInfo.INSTANCE.getRouteMap().remove(msg.getSequence());
        if (record == null) {
            mLogger.severe("no route record was found: " + msg.getSequence());
            return;
        }

        PushMsg.Builder builder = PushMsg.newBuilder(msg);
        builder.setSequence(record.getExtSeq());

        ApiClient apiClient = InMemInfo.INSTANCE.getApiClientMap().getOrDefault(record.getPeerId(), null);
        if (apiClient == null) {
            mLogger.severe("no apiclient was found for id: " + record.getPeerId());
            return;
        }

        apiClient.getChannel().writeAndFlush(builder.build());
    }

    private void handleGetDevInfoRsp(ChannelHandlerContext ctx, PushMsg msg) {
        handleRsp(ctx, msg);
    }

    private void handleGetDevListRsp(ChannelHandlerContext ctx, PushMsg msg) {
        handleRsp(ctx, msg);
    }

    private void handleSetDevInfoRsp(ChannelHandlerContext ctx, PushMsg msg) {
        handleRsp(ctx, msg);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        mLogger.severe(cause.toString());
        mLogger.severe("msg: " + cause.getMessage());
        super.exceptionCaught(ctx, cause);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        mLogger.severe("Channel closed");
        super.channelInactive(ctx);
    }

    public STATE getState() {
        return mState;
    }

    public void setState(STATE mState) {
        this.mState = mState;
    }

}
