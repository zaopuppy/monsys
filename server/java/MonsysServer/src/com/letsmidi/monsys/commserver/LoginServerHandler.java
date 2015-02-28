package com.letsmidi.monsys.commserver;

import java.net.InetSocketAddress;
import java.util.logging.Logger;

import com.letsmidi.monsys.GlobalIdGenerator;
import com.letsmidi.monsys.protocol.commserver.CommServer;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class LoginServerHandler extends SimpleChannelInboundHandler<CommServer.CommServerMsg> {
    private final Logger mLogger = Logger.getLogger(CommConfig.LoggerName);

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        mLogger.info("channelActive()");

        super.channelActive(ctx);

        InMemInfo.INSTANCE.setLoggedIn(false);

        registerToLoginServer(ctx);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        super.channelInactive(ctx);

        InMemInfo.INSTANCE.setLoggedIn(false);
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, CommServer.CommServerMsg msg) throws Exception {
        mLogger.info("channelRead0: " + msg.getType());

        switch (msg.getType()) {
            case REGISTER_RSP:
                if (msg.hasRegisterRsp() && msg.getRegisterRsp().getCode() == 0) {
                    mLogger.info("Registered");
                    InMemInfo.INSTANCE.setLoggedIn(true);
                } else {
                    mLogger.severe("Failed to register to login server");
                    ctx.close();
                }
                break;
            default:
                break;
        }
    }

    private void registerToLoginServer(ChannelHandlerContext ctx) {
        CommServer.CommServerMsg.Builder builder = MsgUtil.newCommServerMsgBuilder(CommServer.MsgType.REGISTER, GlobalIdGenerator.INSTANCE.next());

        CommServer.Register.Builder register = CommServer.Register.newBuilder();
        register.setIpV4Addr("127.0.0.1");
        InetSocketAddress addr = (InetSocketAddress) ctx.channel().localAddress();
        register.setPort(addr.getPort());

        builder.setRegister(register);

        ctx.writeAndFlush(builder.build());
    }

}
