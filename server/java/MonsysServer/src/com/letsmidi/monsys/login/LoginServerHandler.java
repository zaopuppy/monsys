package com.letsmidi.monsys.login;

import java.util.logging.Logger;

import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.protocol.Client;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;
import org.hibernate.ObjectNotFoundException;
import org.hibernate.Session;

public class LoginServerHandler extends SimpleChannelInboundHandler<Client.ClientMsg> {
    private final Logger mLogger = Logger.getLogger(LoginConfig.LoggerName);

    private enum STATE {
        WAITING_FOR_LOGIN,
        LOGGED_IN,
    }

    private STATE mState = STATE.WAITING_FOR_LOGIN;

    //private final SessionManager<Integer> mSessionManager;

    public LoginServerHandler(HashedWheelTimer timer) {
        //mSessionManager = new SessionManager<>(timer);
    }

    private void setState(STATE newState) {
        mState = newState;
    }

    private STATE getState() {
        return mState;
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Client.ClientMsg msg) throws Exception {
        mLogger.info("current state: " + mState.name());
        switch (getState()) {
            case WAITING_FOR_LOGIN:
                onWaitingForLogin(ctx, msg);
                break;
            //case LOGGED_IN:
            //{
            //  onLoggedIn(ctx, msg);
            //  break;
            //}
            default:
                break;
        }
    }

    private void onWaitingForLogin(ChannelHandlerContext ctx, Client.ClientMsg msg) throws Exception {
        mLogger.info("onWaitingForLogin: " + msg.getType());

        switch (msg.getType()) {
            case LOGIN:
                if (!msg.hasLogin()) {
                    mLogger.severe("no login field");
                    ctx.close();
                    break;
                }
                handleLogin(ctx, msg);
                break;
            default:
                mLogger.severe("Unknown message type");
                ctx.close();
                break;
        }

        //Client.Login login = msg.getLogin();
        //PushChannelManager.FgwInfo fgw = new PushChannelManager.FgwInfo(login.getDeviceId(), ctx.channel());
        //if (!PushChannelManager.INSTANCE.add(fgw)) {
        //    ctx.close();
        //    return;
        //}
        //
        //mState = STATE.LOGGED_IN;
        //
        //mLogger.info("Logged in successfully: " + login.getDeviceId());
        //
        //PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.LOGIN_RSP);
        //builder.setSequence(msg.getSequence());
        //
        //LoginRsp.Builder login_rsp = LoginRsp.newBuilder();
        //login_rsp.setCode(0);
        //
        //builder.setLoginRsp(login_rsp);
        //
        //ctx.writeAndFlush(builder.build());
    }

    private void handleLogin(ChannelHandlerContext ctx, Client.ClientMsg msg) {
        mLogger.info("handleLogin");

        Client.Login req = msg.getLogin();

        Session session = HibernateUtil.getSessionFactory().openSession();

        try {
            AccountInfo info = (AccountInfo) session.load(AccountInfo.class, req.getUserName());
            if (info == null) {
                mLogger.severe("bad account");
                ctx.close();
                return;
            }

            if (!info.getPassword().equals(req.getPassword())) {
                mLogger.severe("bad password");
                ctx.close();
                return;
            }

            if (info.getStatus() != AccountInfo.Status.NORMAL.value()) {
                mLogger.severe("account not validated yet");
                ctx.close();
                return;
            }

            sendClientLoginRsp(ctx, msg);

            setState(STATE.LOGGED_IN);
        } catch (ObjectNotFoundException e) {
            mLogger.info("not record found");
            ctx.close();
        } finally {
            session.close();
        }

    }

    private void sendClientLoginRsp(ChannelHandlerContext ctx, Client.ClientMsg req) {
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.LOGIN_RSP);
        builder.setSequence(req.getSequence());

        Client.LoginRsp.Builder rsp = Client.LoginRsp.newBuilder();
        rsp.setCode(0);

        builder.setLoginRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }


    //private void onLoggedIn(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
    //    mLogger.info("onLoggedIn: " + msg.getType());
    //}

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

}
