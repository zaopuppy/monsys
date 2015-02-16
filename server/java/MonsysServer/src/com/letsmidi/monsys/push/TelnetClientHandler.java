package com.letsmidi.monsys.push;

import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.handler.RelayHandler;
import com.letsmidi.monsys.protocol.push.Push.Login;
import com.letsmidi.monsys.protocol.push.Push.MsgType;
import com.letsmidi.monsys.protocol.push.Push.PushMsg;
import com.letsmidi.monsys.util.MsgUtil;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class TelnetClientHandler extends SimpleChannelInboundHandler<String> {
    private static interface Command {
        void exec(ChannelHandlerContext ctx, String[] args);
    }

    private Command DEFAULT_CMD = new Command() {
        @Override
        public void exec(ChannelHandlerContext ctx, String[] args) {
            ctx.write("Unknown command: " + args[0] + "\n");
        }
    };

    private static final Map<String, Command> CMD_MAP = new HashMap<String, Command>();

    private static final String PROMPT = "> ";

    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

    static {
        CMD_MAP.put("connect", new Command() {
            @Override
            public void exec(ChannelHandlerContext ctx, String[] args) {
                if (args.length < 2) {
                    ctx.write("usage: connect <device-id>\n");
                    return;
                }

                String device_id = args[1];
                FgwManager.FgwInfo fgw = FgwManager.INSTANCE.find(device_id);
                if (fgw == null) {
                    ctx.write("gw not connected yet\n");
                    return;
                }

                ctx.write("gw was found\n");

                // gw
                fgw.channel.pipeline().remove(FgwHandler.class);
                // client.channel.pipeline().addLast(new NewServerHandler(ctx.channel()));
                fgw.channel.pipeline().addLast(new RelayHandler(ctx.channel()));

                // client
                ctx.channel().pipeline().remove(ApiHandler.class);
                // ctx.channel().pipeline().addLast(new NewClientHandler(client.channel));
                fgw.channel.pipeline().addLast(new RelayHandler(fgw.channel));
            }
        });
    }

    // --- FOR DEBUGGING ONLY ---
    public static class NewServerHandler extends SimpleChannelInboundHandler<PushMsg> {
        private final Channel mRelayChannel;

        public NewServerHandler(Channel relay) {
            mRelayChannel = relay;
        }

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
            mRelayChannel.writeAndFlush("from gw: " + msg.getType());
        }
    }

    public static class NewClientHandler extends SimpleChannelInboundHandler<String> {
        private final Channel mRelayChannel;

        public NewClientHandler(Channel relay) {
            mRelayChannel = relay;
        }

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, String msg) throws Exception {
            PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(MsgType.LOGIN);

            Login.Builder login = Login.newBuilder();
            login.setDeviceId(msg);

            builder.setLogin(login);

            mRelayChannel.writeAndFlush(builder.build());
        }
    }
    // --- FOR DEBUGGING ONLY ---

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, String msg) throws Exception {
        // TODO Auto-generated method stub
        mLogger.info("ctrl server received: [" + msg + "]");

        String[] args = msg.trim().split("[ \t]+");

        Command cmd = CMD_MAP.get(args[0]);
        if (cmd == null) {
            cmd = DEFAULT_CMD;
        }

        cmd.exec(ctx, args);

        ctx.writeAndFlush(PROMPT);
    }

}

