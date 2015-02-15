package com.letsmidi.monsys.login;

import java.util.logging.Logger;

import com.letsmidi.monsys.database.AccountInfo;
import com.letsmidi.monsys.protocol.client.Client;
import com.letsmidi.monsys.util.HibernateUtil;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;
import org.hibernate.ObjectNotFoundException;
import org.hibernate.Session;

public class ClientHandler extends SimpleChannelInboundHandler<Client.ClientMsg> {
    private final Logger mLogger = Logger.getLogger(LoginConfig.LoggerName);

    private enum STATE {
        WAITING_FOR_LOGIN,
        LOGGED_IN,
    }

    private STATE mState = STATE.WAITING_FOR_LOGIN;

    //private final SessionManager<Integer> mSessionManager;

    public ClientHandler(HashedWheelTimer timer) {
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
        //switch (getState()) {
        //    case WAITING_FOR_LOGIN:
                onWaitingForLogin(ctx, msg);
                //break;
            //case LOGGED_IN:
            //{
            //  onLoggedIn(ctx, msg);
            //  break;
            //}
            //default:
            //    break;
        //}
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
            case REQUEST_COMM_SERVER:
                handleRequestCommServer(ctx, msg);
                break;
            default:
                mLogger.severe("Unknown message type");
                ctx.close();
                break;
        }
    }

    private void handleRequestCommServer(ChannelHandlerContext ctx, Client.ClientMsg msg) {
        InMemInfo.CommServerInfo comm_server_info = InMemInfo.INSTANCE.chooseCommServer();
        sendRequestCommServerRsp(ctx, msg, comm_server_info);
    }

    private void sendRequestCommServerRsp(ChannelHandlerContext ctx, Client.ClientMsg msg, InMemInfo.CommServerInfo info) {
        Client.ClientMsg.Builder builder = MsgUtil.newClientMsgBuilder(Client.MsgType.REQUEST_COMM_SERVER_RSP);

        Client.RequestCommServerRsp.Builder rsp = Client.RequestCommServerRsp.newBuilder();

        if (info == null) {
            mLogger.warning("not comm server available");

            rsp.setIpV4Addr("");
            rsp.setPort(0);
            rsp.setCode(-1);
        } else {
            rsp.setIpV4Addr(info.ipV4Addr);
            rsp.setPort(info.port);
            rsp.setCode(0);
        }

        builder.setRequestCommServerRsp(rsp);

        ctx.writeAndFlush(builder.build());
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
