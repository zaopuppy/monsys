package com.letsmidi.monsys.pushserver;

import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.MsgUtil;
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

    //private final SessionManager<Integer> mSessionManager;

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
        if (!msg.getType().equals(MsgType.PUSH_CLIENT_LOGIN) || !msg.hasLogin()) {
            mLogger.severe("Loggin first please");
            ctx.close();
            return;
        }

        Push.PushClientLogin login = msg.getLogin();
        FgwManager.FgwInfo fgw = new FgwManager.FgwInfo(login.getDeviceId(), ctx.channel());
        if (!FgwManager.INSTANCE.add(fgw)) {
            ctx.close();
            return;
        }

        setState(STATE.LOGGED_IN);

        mLogger.info("Logged in successfully: " + login.getDeviceId());

        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.PUSH_CLIENT_LOGIN_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        Push.PushClientLoginRsp.Builder login_rsp = Push.PushClientLoginRsp.newBuilder();
        login_rsp.setCode(0);

        builder.setLoginRsp(login_rsp);

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
                break;
            case GET_DEV_LIST_RSP:
                break;
            case SET_DEV_INFO_RSP:
                break;
            default:
                break;
        }
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
