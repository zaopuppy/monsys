package com.letsmidi.monsys.login;

import java.net.InetSocketAddress;
import java.util.logging.Logger;

import com.letsmidi.monsys.GlobalIdGenerator;
import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.HashedWheelTimer;


public class CommServerHandler extends SimpleChannelInboundHandler<CommServer.CommServerMsg> {
    private final Logger mLogger = Logger.getLogger(LoginConfig.LoggerName);

    public CommServerHandler(HashedWheelTimer timer) {
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, CommServer.CommServerMsg msg) throws Exception {
        switch (msg.getType()) {
            case REGISTER:
                if (!msg.hasRegister()) {
                    mLogger.severe("no register field");
                    ctx.close();
                    break;
                }
                handleRegister(ctx, msg);
                break;
            default:
                mLogger.severe("Unknown message type");
                ctx.close();
                break;
        }
    }

    private void handleRegister(ChannelHandlerContext ctx, CommServer.CommServerMsg msg) {
        mLogger.info("handleRegister");
        /**
         * Channel.* methods => the operation will start at the tail of the ChannelPipeline
         * ChannelHandlerContext.* methods => the operation will start from this `ChannelHandler`
         * to flow through the ChannelPipeline.
         */
        // save communication server info
        InMemInfo.CommServerInfo info = new InMemInfo.CommServerInfo();
        InetSocketAddress addr = (InetSocketAddress) ctx.channel().localAddress();
        info.ipV4Addr = addr.getHostName();
        info.port = addr.getPort();
        InMemInfo.INSTANCE.getCommServerList().add(info);

        // response
        CommServer.CommServerMsg.Builder builder = MsgUtil.newCommServerMsgBuilder(CommServer.MsgType.REGISTER_RSP, msg.getSequence());

        CommServer.RegisterRsp.Builder register_rsp = CommServer.RegisterRsp.newBuilder();
        register_rsp.setCode(0);

        builder.setRegisterRsp(register_rsp);

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

        InMemInfo.INSTANCE.getCommServerList().remove(ctx.channel());
    }

}
