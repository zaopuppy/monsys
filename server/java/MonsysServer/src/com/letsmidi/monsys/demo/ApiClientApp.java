package com.letsmidi.monsys.demo;

import com.letsmidi.monsys.protocol.push.Push;
import com.letsmidi.monsys.util.BaseClientConnection;
import com.letsmidi.monsys.util.MsgUtil;
import com.letsmidi.monsys.util.SequenceGenerator;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by zhaoyi on 15-6-7.
 */
public class ApiClientApp {

    private static final SequenceGenerator mIdGenerator = new SequenceGenerator(1, 0xFFFFFF);

    private static void log(String msg) {
        System.out.println(msg);
    }

    public static void main(String[] args) throws InterruptedException, IOException {
        NioEventLoopGroup shared_worker = new NioEventLoopGroup();
        ApiClientConnection conn = new ApiClientConnection(shared_worker);

        conn.connect("127.0.0.1", 1988).sync();
        log("connected");

        conn.login().sync();
        log("logged in");

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        for (String line = reader.readLine(); line != null; line = reader.readLine()) {
            if (!conn.channel().isActive()) {
                log("connection break");
                break;
            }
            line = line.trim();
            switch (line) {
                case "dev-list":
                    conn.getDevList();
                    break;
                case "dev-info":
                    conn.getDevInfo();
                    break;
                default:
                    log("unknown command");
                    break;
            }
        }
    }

    private static class ApiClientConnection extends BaseClientConnection<Push.PushMsg> {

        public ApiClientConnection(NioEventLoopGroup group) {
            super(group);
        }

        private SimpleChannelInboundHandler<Push.PushMsg> mHandler = new SimpleChannelInboundHandler<Push.PushMsg>() {
            @Override
            protected void channelRead0(ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
                log("received: " + msg.getType());

                onResponse(msg);
            }
        };

        public ChannelFuture login() {
            ChannelPromise promise = channel().newPromise();


            Push.PushMsg.Builder builder =
                MsgUtil.newPushMsgBuilder(Push.MsgType.ADMIN_CLIENT_LOGIN, mIdGenerator.next());

            Push.AdminClientLogin.Builder login = Push.AdminClientLogin.newBuilder();
            login.setAccount("zao1@gmail.com");
            login.setPassword("password");

            builder.setAdminClientLogin(login);

            writeAndFlush(builder.build(), msg -> {
                if (msg == null || !msg.hasAdminClientLoginRsp()) {
                    log("bad response");
                    promise.setFailure(new Throwable());
                    return;
                }

                Push.AdminClientLoginRsp rsp = msg.getAdminClientLoginRsp();
                if (rsp.getCode() != 0) {
                    log("not success: " + rsp.getCode());
                    promise.setFailure(new Throwable());
                    return;
                }

                promise.setSuccess();
            });

            return promise;
        }

        @Override
        protected void setChannel(Channel channel) {
            super.setChannel(channel);
            if (channel != null) {
                ChannelPipeline pipeline = channel.pipeline();
                pipeline.addLast("header-prepender", new ProtobufVarint32LengthFieldPrepender());
                pipeline.addLast("frame-decoder", new ProtobufVarint32FrameDecoder());
                pipeline.addLast("encoder", new ProtobufEncoder());
                pipeline.addLast("decoder", new ProtobufDecoder(
                        Push.PushMsg.getDefaultInstance()));
                pipeline.addLast("handler", mHandler);
            }
        }

        @Override
        public Channel popChannel() {
            Channel channel = channel();
            if (channel == null) {
                return null;
            }

            ChannelPipeline pipeline = channel.pipeline();

            pipeline.remove("header-prepender");
            pipeline.remove("frame-decoder");
            pipeline.remove("encoder");
            pipeline.remove("decoder");
            pipeline.remove("handler");

            return channel;
        }

        @Override
        protected boolean saveRoute(Push.PushMsg msg, Callback<Push.PushMsg> callback) {
            getRouteMap().put(msg.getSequence(), new RouteItem<>(msg.getSequence(), callback));
            return true;
        }

        @Override
        protected RouteItem<Push.PushMsg> removeRoute(Push.PushMsg msg) {
            return getRouteMap().remove(msg.getSequence());
        }

        public void getDevInfo() {
            Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_DEV_INFO, mIdGenerator.next());

            Push.GetDevInfo.Builder get_dev_info = Push.GetDevInfo.newBuilder();
            get_dev_info.setDeviceId("any-devices");
            get_dev_info.setAddr(123);
            get_dev_info.addItemIds(0);

            builder.setGetDevInfo(get_dev_info);

            writeAndFlush(builder.build(), msg -> {
                if (msg == null || !msg.hasGetDevInfoRsp()) {
                    log("bad response");
                    return;
                }

                Push.GetDevInfoRsp get_dev_info_rsp = msg.getGetDevInfoRsp();
                log("response code: " + get_dev_info_rsp.getCode());
                log("response size: " + get_dev_info_rsp.getIdValuePairsList().size());
                for (Push.IdValuePair pair : get_dev_info_rsp.getIdValuePairsList()) {
                    log("id=" + pair.getId() + ", value=" + pair.getValue());
                }
            });
        }

        public void getDevList() {
            Push.PushMsg.Builder builder = MsgUtil.newPushMsgBuilder(Push.MsgType.GET_DEV_LIST, mIdGenerator.next());

            Push.GetDevList.Builder get_dev_list = Push.GetDevList.newBuilder();
            get_dev_list.setDeviceId("any-devices");

            builder.setGetDevList(get_dev_list);

            writeAndFlush(builder.build(), msg -> {
                if (msg == null || !msg.hasGetDevListRsp()) {
                    log("bad response");
                    return;
                }

                Push.GetDevListRsp get_dev_list_rsp = msg.getGetDevListRsp();
                log("response code: " + get_dev_list_rsp.getCode());
                log("response size: " + get_dev_list_rsp.getDevInfosList().size());
                for (Push.DeviceInfo info: get_dev_list_rsp.getDevInfosList()) {
                    log("name=" + info.getName() + ", type=" + info.getType() + ", addr=" + info.getAddr());
                }
            });
        }
    }
}
