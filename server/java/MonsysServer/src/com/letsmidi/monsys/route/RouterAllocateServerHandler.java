package com.letsmidi.monsys.route;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.log.Logger;
import com.letsmidi.monsys.protocol.route.Route;
import com.letsmidi.monsys.protocol.route.Route.RouteMsg;
import com.letsmidi.monsys.route.session.RouteSession;
import com.letsmidi.monsys.session.BaseSession;
import com.letsmidi.monsys.session.SessionManager;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

/**
 * http://stackoverflow.com/questions/1626575/best-practices-around-generating-oauth-tokens
 * token generation: enrypt/sumary(key+expiration)
 * <p>
 * for access server, it needs: token,
 *
 * @author zero
 */
public class RouterAllocateServerHandler extends SimpleChannelInboundHandler<Route.RouteMsg> {
    private static final String TAG = "RouterAllocateServerHandler";

    private final SessionManager mSessionManager;

    public RouterAllocateServerHandler(SessionManager session_manager) {
        mSessionManager = session_manager;
    }

    /**
     * XXX: authentication is needed, event for internal module
     */
    @Override
    protected void channelRead0(ChannelHandlerContext ctx, RouteMsg msg) throws Exception {
        switch (msg.getType()) {
            case REQ_TOKEN:
                processTokenRequest(ctx, msg);
                break;
            default:
                Logger.e(TAG, "bad request");
                ctx.close();
                break;
        }
    }

    private void processTokenRequest(ChannelHandlerContext ctx, RouteMsg msg) {
        if (msg.getType() != Route.MsgType.REQ_TOKEN || !msg.hasReqToken()) {
            sendTokenResponse(ctx, -1, -1, -1);
            return;
        }

        Route.ReqToken req = msg.getReqToken();

        BaseSession session = null;
        // BaseSession session = mSessionManager.find(req.getToken());
        if (session != null) {
            session.close(); // this should remove itself automatically from SessionManager.
        }

        RouteSession route_session = new RouteSession(mSessionManager, req.getToken());
        route_session.client_ip_v4 = req.getClientIpV4();
        route_session.fgw_ip_v4 = req.getFgwIpV4();

        mSessionManager.add(route_session);

        sendTokenResponse(ctx, 0,
                Config.getRouterConfig().getAllocatePort(),
                Config.getRouterConfig().getAccessPort());
    }

    private void sendTokenResponse(ChannelHandlerContext ctx, int code, int port_for_client, int port_for_fgw) {
        RouteMsg.Builder builder = MsgUtil.newRouteMsgBuilder(Route.MsgType.REQ_TOKEN_RSP, 0);

        Route.ReqTokenRsp.Builder token_rsp = Route.ReqTokenRsp.newBuilder();
        token_rsp.setCode(code);
        token_rsp.setPortForClient(port_for_client);
        token_rsp.setPortForFgw(port_for_fgw);

        builder.setReqTokenRsp(token_rsp);

        ctx.writeAndFlush(builder.build());
    }

}
