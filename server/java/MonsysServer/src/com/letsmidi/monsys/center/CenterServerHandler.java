package com.letsmidi.monsys.center;

import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.protocol.center.Center;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import org.hibernate.Session;

/**
 * Created by zero on 8/16/14.
 */
public class CenterServerHandler extends SimpleChannelInboundHandler<Center.CenterMsg> {
    private enum State {
        STATE_WAIT_FOR_LOGIN,
        STATE_LOGGED_IN,
    }

    private State mState = State.STATE_WAIT_FOR_LOGIN;

    private Logger mLogger = Logger.getLogger(Config.getCenterConfig().getLoggerName());

    private String[] mFgwList = null;

    private void setState(State newState) {
        mState = newState;
    }

    private State getState() {
        return mState;
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Center.CenterMsg msg) throws Exception {
        switch (getState()) {
            case STATE_WAIT_FOR_LOGIN:
                onRead_WaitForLogin(ctx, msg);
                break;
            case STATE_LOGGED_IN:
                onRead_LoggedIn(ctx, msg);
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


    private void onRead_WaitForLogin(ChannelHandlerContext ctx, Center.CenterMsg msg) {
        mLogger.info("onRead_WaitForLogin()");
        switch (msg.getType()) {
            //case USER_REGISTER:
            //  processUserRegister(ctx, msg);
            //  break;
            case CLIENT_LOGIN:
                processClientLogin(ctx, msg);
                break;
            default:
                mLogger.severe("Unknown message: " + msg.getType());
                ctx.close();
                break;
        }
    }

    private void onRead_LoggedIn(ChannelHandlerContext ctx, Center.CenterMsg msg) {
        mLogger.info("onRead_LoggedIn()");
        switch (msg.getType()) {
            case CONNECT:
                processConnect(ctx, msg);
                break;
            case GET_FGW_LIST:
                processGetFgwList(ctx, msg);
                break;
            default:
                mLogger.severe("Unknown message: " + msg.getType());
                ctx.close();
                break;
        }
    }

    private void processGetFgwList(ChannelHandlerContext ctx, Center.CenterMsg msg) {
        mLogger.info("processGetFgwList");
        if (!msg.hasGetFgwList()) {
            return;
        }

        if (mFgwList != null) {
            sendGetFgwListRsp(ctx, msg, mFgwList);
            return;
        }

        // query from push server (of course we should cache the response of course...)
        ServiceManager.Service service = ServiceManager.INSTANCE.get("push");
        if (service == null) {
            //
            return;
        }

        service.send(msg);


        //Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_FGW_LIST);
        //builder.setSequence(msg.getSequence());
        //
        //Push.GetFgwList.Builder request = Push.GetFgwList.newBuilder();
        //
        //builder.setGetFgwList(request);
        //
        //service.writeAndFlushInEventLoop(builder.build());
    }

    private void sendGetFgwListRsp(ChannelHandlerContext ctx, Center.CenterMsg msg, String[] fgw_list) {
        Center.CenterMsg.Builder builder = MsgUtil.newCenterMsgBuilder(Center.MsgType.GET_FGW_LIST_RSP, msg.getSequence());
        builder.setSequence(builder.getSequence());

        Center.GetFgwListRsp.Builder rsp = Center.GetFgwListRsp.newBuilder();
        rsp.setCode(0);
        Center.FGatewayInfo.Builder info = Center.FGatewayInfo.newBuilder();
        for (String fgw : fgw_list) {
            info.setId(fgw);
            info.setName(fgw);
            info.setDesc("nothing about this gateway");
            rsp.addFgwInfos(info.build());
        }

        builder.setGetFgwListRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }

    private void processConnect(ChannelHandlerContext ctx, Center.CenterMsg msg) {
    }

    private void processClientLogin(ChannelHandlerContext ctx, Center.CenterMsg msg) {
        mLogger.info("processClientLogin()");

        if (msg.getType() != Center.MsgType.CLIENT_LOGIN || !msg.hasClientLogin()) {
            mLogger.severe("ClientLogin expected, but received: " + msg.getType());
            ctx.close();
            return;
        }

        Center.ClientLogin login = msg.getClientLogin();

        Session session = HibernateUtil.getSessionFactory().openSession();

        try {
            AccountInfo info = (AccountInfo) session.get(AccountInfo.class, login.getAccount());
            if (info == null) {
                mLogger.severe("bad account");
                ctx.close();
                return;
            }

            if (!info.getPassword().equals(login.getPassword())) {
                mLogger.severe("bad password");
                ctx.close();
                return;
            }

            if (info.getStatus() != AccountInfo.Status.NORMAL.value()) {
                mLogger.severe("account not validated yet");
                ctx.close();
                return;
            }

            //query fgw list from Push server
            //String fgw_list = info.getFgwList();

            //this.mFgwList = fgw_list;

            sendClientLoginRsp(ctx, msg);

            setState(State.STATE_LOGGED_IN);

        } finally {
            session.close();
        }
    }

    private void sendClientLoginRsp(ChannelHandlerContext ctx, Center.CenterMsg msg) {
        Center.CenterMsg.Builder builder = MsgUtil.newCenterMsgBuilder(Center.MsgType.CLIENT_LOGIN_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        Center.ClientLoginRsp.Builder rsp = Center.ClientLoginRsp.newBuilder();
        rsp.setCode(0);

        builder.setClientLoginRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }


}
