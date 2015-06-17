package com.letsmidi.monsys.performance.test;

import com.letsmidi.monsys.protocol.push.Push;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import java.util.concurrent.TimeUnit;

/**
 * Created by zero on 9/8/14.
 */
public class FgwStubTest extends ConcurrentTest {

  private StatData mStatData = null;

  public FgwStubTest(String host, int port, int connection_count) {
    super(host, port, connection_count);
  }

  @Override
  public void start() {
    mStatData = new StatData();
    mStatData.initialTime.set(System.currentTimeMillis());
    super.start();
  }

  @Override
  public ChannelHandler newHandler() {
    return new Handler(mStatData, genNumericDeviceId());
  }

  private int mLastDeviceId = 1;
  private String genNumericDeviceId() {
    return String.format("%025d", mLastDeviceId++);
  }

  private static class Handler extends SimpleChannelInboundHandler<Push.PushMsg> {
    private final StatData mStatData;

    private final long mBeginTime;

    private boolean mLoggedIn = false;

    private final String mDeviceId;

    public Handler(StatData stat_data, String device_id) {
      mBeginTime = System.currentTimeMillis();

      mStatData = stat_data;

      mDeviceId = device_id;

      // ++
      mStatData.createdConnections.incrementAndGet();
      mStatData.waitingConnections.incrementAndGet();
      mStatData.createUsedTime.set(mBeginTime - mStatData.initialTime.get());

      printCount(mStatData);

    }

    public final char[] ALPHA_MAP = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".toCharArray();
    public String genRandomString(int len) {
      StringBuilder builder = new StringBuilder(len+5);

      int idx;
      for (int i = 0; i < len; ++i) {
        idx = (int) Math.round(Math.random()*(ALPHA_MAP.length-1));
        builder.append(ALPHA_MAP[idx]);
      }

      return builder.toString();
    }

    private void login(ChannelHandlerContext ctx) {
      Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
      builder.setVersion(1);
      builder.setType(Push.MsgType.PUSH_CLIENT_LOGIN);
      builder.setSequence(1);

      Push.PushClientLogin.Builder login = Push.PushClientLogin.newBuilder();
      // login.setDeviceId(genRandomString(25));
      login.setDeviceId(mDeviceId);

      builder.setPushClientLogin(login);

      ctx.writeAndFlush(builder.build());
    }

    private void printCount(StatData stat_data) {
      System.out.println(stat_data.toString());
    }

    @Override
    protected void channelRead0(final ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
      // do nothing
      if (!mLoggedIn) {
        if (msg.getType() != Push.MsgType.PUSH_CLIENT_LOGIN_RSP && !msg.hasPushClientLoginRsp()) {
          System.out.println("unknown message");
          ctx.close();
          return;
        }

        Push.PushClientLoginRsp rsp = msg.getPushClientLoginRsp();
        if (rsp.getCode() != 0) {
          System.out.println("failed response");
          ctx.close();
          return;
        }
        mLoggedIn = true;

      } else {
        switch (msg.getType()) {
          case GET_FGW_LIST: {
            onGetFgwList(ctx, msg);
            break;
          }
          case GET_DEV_INFO: {
            onGetDevInfo(ctx, msg);
            break;
          }
          case SET_DEV_INFO: {
            onSetDevInfo(ctx, msg);
            break;
          }
          default:
            break;
        }
      }
    }

    private void onSetDevInfo(ChannelHandlerContext ctx, Push.PushMsg msg) {
      //
    }

    private void onGetDevInfo(ChannelHandlerContext ctx, Push.PushMsg msg) {
      //
    }

    private void onGetFgwList(ChannelHandlerContext ctx, Push.PushMsg msg) {
      Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
      builder.setVersion(1);
      builder.setType(Push.MsgType.GET_FGW_LIST_RSP);
      builder.setSequence(msg.getSequence());

      Push.GetFgwListRsp.Builder rsp = Push.GetFgwListRsp.newBuilder();

      Push.FGatewayInfo.Builder info = Push.FGatewayInfo.newBuilder();
      info.setName("Fgw0");
      info.setId(mDeviceId);
      info.setDesc("stub fgw device");
      rsp.addFgwInfos(info.build());

      builder.setGetFgwListRsp(rsp);

      ctx.writeAndFlush(builder.build());
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
      super.channelActive(ctx);

      int delay = (int)(System.currentTimeMillis() - mBeginTime);

      mStatData.addDelay(delay);
      mStatData.establishedConnections.incrementAndGet();
      mStatData.waitingConnections.decrementAndGet();
      printCount(mStatData);

      login(ctx);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
      super.channelInactive(ctx);
      mStatData.establishedConnections.decrementAndGet();
      mStatData.disconnectedConnections.incrementAndGet();
      printCount(mStatData);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
      cause.printStackTrace();
      ctx.close();
    }
  }
}
