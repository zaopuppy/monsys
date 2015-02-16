package com.letsmidi.monsys.push;

import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.GlobalIdGenerator;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.push.Push.ClientLoginRsp;
import com.letsmidi.monsys.protocol.push.Push.Connect;
import com.letsmidi.monsys.protocol.push.Push.ConnectRsp;
import com.letsmidi.monsys.protocol.push.Push.FGatewayInfo;
import com.letsmidi.monsys.protocol.push.Push.GetFgwListRsp;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.protocol.push.Push.UserRegisterRsp;
import com.letsmidi.monsys.push.session.PushSession;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;

/**
 * TODO: common response
 *
 * @author zero
 */
public class ApiHandler extends SimpleChannelInboundHandler<PushMsg> {

    //public static class ApiSession extends BaseSession<Integer> {
    //    public int external_sequence = -1;
    //
    //    public ApiSession(SessionManager manager, Integer key) {
    //        super(manager, key);
    //    }
    //}

    private static final int RECONNECT_WAIT_TIME = 5; // in seconds

    private String mFgwList = null;

    private final HashedWheelTimer mTimer;
    //private final SessionManager<Integer, ApiSession> mSessionManager;
    //private final SessionManager<Integer, ApiSession> mInnerSessionManager;
    //private SequenceGenerator mSequenceGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());
    private FgwManager.FgwInfo mCurrentFgw = null;

    public ApiHandler(HashedWheelTimer timer) {
        mTimer = timer;
        //mSessionManager = new SessionManager<>(timer);
        //mInnerSessionManager = new SessionManager<>(timer);
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        super.channelActive(ctx);
        mLogger.info("connected from: " + ctx.channel().remoteAddress());
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        super.channelInactive(ctx);
        mLogger.info("disconnected from: " + ctx.channel().remoteAddress());
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
        switch (msg.getType()) {
            case HEARTBEAT:
                break;
            case CONNECT:
                handleConnectReq(ctx, msg);
                break;
            case GET_FGW_LIST:
                handleFgwListReq(ctx, msg);
                break;
            case GET_DEV_LIST:
                handleDevListReq(ctx, msg);
                break;
            case GET_DEV_INFO:
                break;
            case SET_DEV_INFO:
                break;
            default:
                mLogger.severe("unknown type: " + msg.getType());
                ctx.close();
                break;
        }
    }

    private void handleDevListReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasGetDevList()) {
            return;
        }

        Push.GetDevList get_dev_list = msg.getGetDevList();
        // TODO: re-connect or check if device id is correct
        String device_id = get_dev_list.getDeviceId();
        if (mCurrentFgw == null || !mCurrentFgw.deviceId.equals(device_id)) {
            mCurrentFgw = FgwManager.INSTANCE.find(device_id);
        }

        if (mCurrentFgw == null) {
            return;
        }

        Channel ch = mCurrentFgw.channel;
        if (ch == null || !ch.isActive()) {
            return;
        }

        int external_sequence = msg.getSequence();
        int internal_sequence = GlobalIdGenerator.INSTANCE.next();

        PushSession session = new PushSession(InMemInfo.INSTANCE.getInternalSessionManager(), internal_sequence);
        session.external_sequence = external_sequence;

        // add to session
        InMemInfo.INSTANCE.getInternalSessionManager().add(session);

        // modify sequence
        PushMsg.Builder builder = PushMsg.newBuilder();
        builder.setSequence(internal_sequence);

        // send to FGW
        ch.writeAndFlush(builder.build());
    }

    private void handleFgwListReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasGetFgwList()) {
            return;
        }

        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.GET_FGW_LIST_RSP, msg.getSequence());
        Push.GetFgwListRsp.Builder get_fgw_list_rsp = Push.GetFgwListRsp.newBuilder();
        get_fgw_list_rsp.setCode(-1);
        builder.setGetFgwListRsp(get_fgw_list_rsp);

        ctx.writeAndFlush(builder.build());
    }

    private void handleConnectReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasConnect()) {
            mLogger.severe("no connect req");
            ctx.close();
            return;
        }

        Connect connect = msg.getConnect();
        String device_id = connect.getDeviceId();
        FgwManager.FgwInfo info = FgwManager.INSTANCE.find(device_id);
        if (info == null) {
            mLogger.warning("no such device");
            sendConnectRsp(ctx, msg, -1);
            return;
        }

        mCurrentFgw = info;

        sendConnectRsp(ctx, msg, 0);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        mLogger.severe(cause.toString());
        mLogger.severe("msg: " + cause.getMessage());
        super.exceptionCaught(ctx, cause);
    }

    //private void onRead_WaitForLogin(ChannelHandlerContext ctx, PushMsg msg) {
    //    mLogger.info("onRead_WaitForLogin()");
    //    switch (msg.getType()) {
    //        case USER_REGISTER:
    //            processUserRegister(ctx, msg);
    //            break;
    //        case CLIENT_LOGIN:
    //            processClientLogin(ctx, msg);
    //            break;
    //        default:
    //            mLogger.severe("Unknown message: " + msg.getType());
    //            ctx.close();
    //            break;
    //    }
    //}

    //private void onRead_LoggedIn(ChannelHandlerContext ctx, PushMsg msg) {
    //    mLogger.info("onRead_LoggedIn()");
    //    switch (msg.getType()) {
    //        case CONNECT:
    //            processConnect(ctx, msg);
    //            break;
    //        case GET_FGW_LIST:
    //            processGetFgwList(ctx, msg);
    //            break;
    //        default:
    //            mLogger.severe("Unknown message: " + msg.getType());
    //            ctx.close();
    //            break;
    //    }
    //}

    //private void processUserRegister(ChannelHandlerContext ctx, PushMsg msg) {
    //    if (msg.getType() != MsgType.USER_REGISTER || !msg.hasUserRegister()) {
    //        mLogger.severe("Bad request: " + msg.getType());
    //        ctx.close();
    //        return;
    //    }
    //
    //    UserRegister user_register = msg.getUserRegister();
    //
    //    // check arguments
    //    if (user_register.getAccount().length() < 3 ||
    //            user_register.getPassword().length() < 3 ||
    //            user_register.getNickname().length() < 3) {
    //        mLogger.severe("Bad parameters");
    //        ctx.close();
    //        return;
    //    }
    //
    //    Session session = HibernateUtil.getSessionFactory().openSession();
    //
    //    try {
    //        AccountInfo account_info =
    //                (AccountInfo) session.get(AccountInfo.class, user_register.getAccount());
    //        if (account_info != null) {
    //            mLogger.warning("account already exists: " + user_register.getAccount());
    //            ctx.close();
    //            return;
    //        }
    //
    //        // TODO: mail server is needed for mail validation
    //        //TempAccountInfo temp_account_info =
    //        //    (TempAccountInfo) session.load(TempAccountInfo.class, user_register.getAccount());
    //        //if (temp_account_info != null) {
    //        //  mLogger.warning("this account is waiting for activate");
    //        //  ctx.close();
    //        //  return;
    //        //}
    //        //
    //        //mLogger.info("account doesn't exist");
    //        //
    //        //// insert into temp table
    //        //temp_account_info = new TempAccountInfo();
    //        //temp_account_info.setAccount(user_register.getAccount());
    //        //temp_account_info.setPassword(user_register.getPassword());
    //        //temp_account_info.setNickname(user_register.getNickname());
    //        //temp_account_info.setSalt("abc");
    //        //temp_account_info.setExpire(new Date());
    //        //session.beginTransaction();
    //        //session.save(temp_account_info);
    //        //session.getTransaction().commit();
    //        //
    //        //// send mail (use message queue)
    //        //sendEmail();
    //
    //        account_info = new AccountInfo();
    //        account_info.setAccount(user_register.getAccount());
    //        account_info.setPassword(user_register.getPassword());
    //        account_info.setNickname(user_register.getNickname());
    //        account_info.setFgwList("");
    //        account_info.setSalt("abc");
    //        account_info.setStatus(AccountInfo.Status.NORMAL.value());
    //
    //        session.beginTransaction();
    //        session.save(account_info);
    //        session.getTransaction().commit();
    //
    //        mLogger.info("new account added");
    //
    //        sendUserRegisterRsp(ctx, msg, 0);
    //
    //        // done
    //    } finally {
    //        session.close();
    //    }
    //}

    // TODO: use stand-alone thread/process/module instead of sending directly
    //private void sendEmail() {
    //  String validate_url = "";
    //}

    private void processGetFgwList(ChannelHandlerContext ctx, PushMsg msg) {
        mLogger.info("processGetFgwList");
        if (!msg.hasGetFgwList()) {
            return;
        }

        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.GET_FGW_LIST_RSP);
        builder.setSequence(builder.getSequence());

        GetFgwListRsp.Builder rsp = GetFgwListRsp.newBuilder();
        rsp.setCode(0);
        FGatewayInfo.Builder info = FGatewayInfo.newBuilder();
        if (this.mFgwList != null) {
            String[] fgws = this.mFgwList.split("\\|");
            for (String fgw : fgws) {
                info.setId(fgw);
                info.setName(fgw);
                info.setDesc("nothing about this gateway");
                rsp.addFgwInfos(info.build());
            }
        }

        builder.setGetFgwListRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }

    private class WaitForFgwTask implements Runnable {
        private final String mDeviceId;
        private final ChannelHandlerContext mContext;
        private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());
        private int mRetryTimes;
        private final PushMsg mMsg;

        public WaitForFgwTask(ChannelHandlerContext ctx, PushMsg msg, String device_id, int retry_times) {
            mContext = ctx;
            mDeviceId = device_id;
            mRetryTimes = retry_times;
            mMsg = msg;
        }

        @Override
        public void run() {
            if (mRetryTimes <= 0) {
                mLogger.warning("Timeout");
                mContext.close();
                return;
            }
            --mRetryTimes;

            FgwManager.FgwInfo fgw_again = FgwManager.INSTANCE.find(mDeviceId);
            if (fgw_again == null) {
                mLogger.severe("fgw still not connected");
                // mContext.close();
                mContext.executor().schedule(this, RECONNECT_WAIT_TIME, TimeUnit.SECONDS);
                return;
            }

            if (!fgw_again.connected.compareAndSet(false, true)) {
                mLogger.severe("Still not fast enough, fail");
                mContext.close();
                return;
            }

            setupRelayChannels(mContext, mMsg, fgw_again);
        }
    }

    private void processConnect(final ChannelHandlerContext ctx, PushMsg msg) {
        mLogger.info("processConnect()");

        if (msg.getType() != MsgType.CONNECT || !msg.hasConnect()) {
            mLogger.severe("connect expected, but received: " + msg.getType());
            ctx.close();
            return;
        }

        Connect connect_req = msg.getConnect();

        final String device_id = connect_req.getDeviceId();

        FgwManager.FgwInfo fgw = FgwManager.INSTANCE.find(device_id);
        if (fgw == null) {
            mLogger.severe("gw not connected yet");
            ctx.close();
            return;
        }

        mLogger.info("gw was found");

        if (!fgw.connected.compareAndSet(false, true)) {
            // close and wait
            mLogger.info("close existing client channel then wait");
            fgw.channel.close();
            // ctx.executor().schedule(new WaitForFgwTask(ctx, device_id, 2), RECONNECT_WAIT_TIME, TimeUnit.SECONDS);
            new WaitForFgwTask(ctx, msg, device_id, 2).run();
            return;
        }

        setupRelayChannels(ctx, msg, fgw);
    }

    //private void setupRelayChannels(final ChannelHandlerContext ctx, PushMsg msg, FgwManager.FgwInfo fgw) {
    //    fgw.channel.pipeline().remove(FgwHandler.class);
    //    fgw.channel.pipeline().addLast(new FgwRelayHandler(ctx.channel()));
    //
    //    ctx.channel().pipeline().remove(ApiHandler.class);
    //    ClientRelayHandler handler = new ClientRelayHandler(fgw.channel);
    //    handler.setFgwList(this.mFgwList);
    //    ctx.channel().pipeline().addLast(handler);
    //
    //    sendConnectRsp(ctx, msg, 0);
    //}

    //private void processClientLogin(ChannelHandlerContext ctx, PushMsg msg) {
    //    mLogger.info("processClientLogin()");
    //
    //    if (msg.getType() != MsgType.CLIENT_LOGIN || !msg.hasClientLogin()) {
    //        mLogger.severe("ClientLogin expected, but received: " + msg.getType());
    //        ctx.close();
    //        return;
    //    }
    //
    //    ClientLogin login = msg.getClientLogin();
    //
    //    Session session = HibernateUtil.getSessionFactory().openSession();
    //
    //    try {
    //        AccountInfo info = (AccountInfo) session.load(AccountInfo.class, login.getAccount());
    //        if (info == null) {
    //            mLogger.severe("bad account");
    //            ctx.close();
    //            return;
    //        }
    //
    //        if (!info.getPassword().equals(login.getPassword())) {
    //            mLogger.severe("bad password");
    //            ctx.close();
    //            return;
    //        }
    //
    //        if (info.getStatus() != AccountInfo.Status.NORMAL.value()) {
    //            mLogger.severe("account not validated yet");
    //            ctx.close();
    //            return;
    //        }
    //
    //        String fgw_list = info.getFgwList();
    //
    //        this.mFgwList = fgw_list;
    //
    //        sendClientLoginRsp(ctx, msg, fgw_list);
    //
    //        //setState(State.STATE_LOGGED_IN);
    //
    //    } finally {
    //        session.close();
    //    }
    //
    //}

    private void sendClientLoginRsp(ChannelHandlerContext ctx, PushMsg msg, String fgw_list) {
        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.CLIENT_LOGIN_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        ClientLoginRsp.Builder rsp = ClientLoginRsp.newBuilder();
        rsp.setCode(0);

        builder.setClientLoginRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }

    private void sendConnectRsp(ChannelHandlerContext ctx, PushMsg msg, int code) {
        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.CONNECT_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        ConnectRsp.Builder rsp = ConnectRsp.newBuilder();
        rsp.setCode(code);

        builder.setConnectRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }

    private void sendUserRegisterRsp(ChannelHandlerContext ctx, PushMsg msg, int code) {
        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.USER_REGISTER_RSP, msg.getSequence());
        builder.setSequence(msg.getSequence());

        UserRegisterRsp.Builder rsp = UserRegisterRsp.newBuilder();
        rsp.setCode(code);

        builder.setUserRegisterRsp(rsp);

        ctx.writeAndFlush(builder.build());
    }
}
