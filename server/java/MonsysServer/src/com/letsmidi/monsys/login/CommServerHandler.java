package com.letsmidi.monsys.login;

import java.util.logging.Logger;

import com.letsmidi.monsys.protocol.commserver.CommServer;
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
         * Channel.* methods ⇒ the operation will start at the tail of the ChannelPipeline
         * ChannelHandlerContext.* methods => the operation will start from this `ChannelHandler`
         * to flow through the ChannelPipeline.
         */
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
