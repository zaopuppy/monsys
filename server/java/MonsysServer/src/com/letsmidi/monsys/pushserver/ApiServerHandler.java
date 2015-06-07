package com.letsmidi.monsys.pushserver;

import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.protocol.push.Push.ConnectRsp;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;

/**
 * TODO: common response
 *
 * @author Yi Zhao
 */
public class ApiServerHandler extends SimpleChannelInboundHandler<PushMsg> {

    private enum State {
        WAIT_FOR_LOGIN,
        LOGGED_IN,
    }

    private State mState = State.WAIT_FOR_LOGIN;

    private String mId;

    private SequenceGenerator mIdGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

    public ApiServerHandler(HashedWheelTimer timer) {
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
        switch (mState) {
            case WAIT_FOR_LOGIN:
                onWaitForLogin(ctx, msg);
                break;
            case LOGGED_IN:
                onLoggedIn(ctx, msg);
                break;
            default:
                mLogger.severe("bad state");
                ctx.close();
                break;
        }
    }

    private void onWaitForLogin(ChannelHandlerContext ctx, PushMsg msg) {
        switch (msg.getType()) {
            case ADMIN_CLIENT_LOGIN:
                handleAdminClientLogin(ctx, msg);
                break;
            default:
                mLogger.severe("unknown type: " + msg.getType());
                ctx.close();
                break;
        }
    }

    private void onLoggedIn(ChannelHandlerContext ctx, PushMsg msg) {
        switch (msg.getType()) {
            case HEARTBEAT:
                break;
            case GET_FGW_LIST:
                handleFgwListReq(ctx, msg);
                break;
            case GET_DEV_LIST:
                handleDevListReq(ctx, msg);
                break;
            case GET_DEV_INFO:
                handleDevInfoReq(ctx, msg);
                break;
            case SET_DEV_INFO:
                handleSetDevInfoReq(ctx, msg);
                break;
            default:
                mLogger.severe("unknown type: " + msg.getType());
                ctx.close();
                break;
        }
    }

    private void handleAdminClientLogin(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasAdminClientLogin()) {
            mLogger.severe("no admin client login");
            ctx.close();
            return;
        }

        Push.AdminClientLogin login = msg.getAdminClientLogin();

        mId = login.getAccount();

        // add to map first
        InMemInfo.INSTANCE.getApiClientMap().put(mId, new ApiClient(mId, ctx.channel()));
        ctx.channel().closeFuture().addListener(future -> InMemInfo.INSTANCE.getApiClientMap().remove(mId));

        // build response
        PushMsg.Builder builder = PushMsg.newBuilder();
        builder.setType(MsgType.ADMIN_CLIENT_LOGIN_RSP);
        builder.setSequence(msg.getSequence());

        Push.AdminClientLoginRsp.Builder loginRsp = Push.AdminClientLoginRsp.newBuilder();
        loginRsp.setCode(0);

        builder.setAdminClientLoginRsp(loginRsp);

        ctx.writeAndFlush(builder.build());

        // transfer state
        mState = State.LOGGED_IN;
    }

    // TODO: duplicated code
    private void handleSetDevInfoReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasSetDevInfo()) {
            return;
        }

        Push.SetDevInfo set_dev_info = msg.getSetDevInfo();

        convertAndRoute(ctx, msg, set_dev_info.getDeviceId());
    }

    private void handleDevInfoReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasGetDevInfo()) {
            return;
        }

        Push.GetDevInfo get_dev_info = msg.getGetDevInfo();

        convertAndRoute(ctx, msg, get_dev_info.getDeviceId());
    }

    private void handleDevListReq(ChannelHandlerContext ctx, PushMsg msg) {
        if (!msg.hasGetDevList()) {
            return;
        }

        Push.GetDevList get_dev_list = msg.getGetDevList();

        String device_id = get_dev_list.getDeviceId();

        convertAndRoute(ctx, msg, device_id);
    }

    private void convertAndRoute(ChannelHandlerContext ctx, PushMsg msg, String peerId) {
        PushClient client = InMemInfo.INSTANCE.getPushClientMap().getOrDefault(peerId, null);
        //FgwManager.FgwInfo fgwInfo = FgwManager.INSTANCE.find(peerId);
        //if (fgwInfo == null) {
        if (client == null) {
            mLogger.severe("no device was found");
            return;
        }

        int ext_seq = msg.getSequence();
        int inner_seq = mIdGenerator.next();

        // save route first
        InMemInfo.INSTANCE.getRouteMap().put(inner_seq, new RouteRecord(inner_seq, ext_seq, mId));

        // modify sequence
        PushMsg.Builder builder = msg.toBuilder();
        builder.setSequence(inner_seq);

        // send to FGW
        client.getChannel().writeAndFlush(builder.build());
    }

    private void handleFgwListReq(ChannelHandlerContext ctx, PushMsg msg) {
        mLogger.severe("not implemented yet");
        if (!msg.hasGetFgwList()) {
            return;
        }

        // TODO
        PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.GET_FGW_LIST_RSP, msg.getSequence());
        Push.GetFgwListRsp.Builder get_fgw_list_rsp = Push.GetFgwListRsp.newBuilder();
        get_fgw_list_rsp.setCode(0);
        Push.FGatewayInfo.Builder info = Push.FGatewayInfo.newBuilder();
        info.setId("ID-01");
        info.setName("ID-01-NAME");
        info.setDesc("THIS IS A FAKE DATA, FOR DEMONSTRATION ONLY");
        get_fgw_list_rsp.addFgwInfos(info.build());
        builder.setGetFgwListRsp(get_fgw_list_rsp);

        ctx.writeAndFlush(builder.build());
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        mLogger.severe(cause.toString());
        mLogger.severe("msg: " + cause.getMessage());
        super.exceptionCaught(ctx, cause);
    }
}
