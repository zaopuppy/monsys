package com.letsmidi.monsys.performance.test;

import com.letsmidi.monsys.protocol.push.Push;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import java.util.concurrent.TimeUnit;

/**
 * Created by zero on 9/8/14.
 */
public class ClientStubTest extends ConcurrentTest {

  private StatData mStatData = null;

  public ClientStubTest(String host, int port, int connection_count) {
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
    return new Handler(mStatData);
  }

  private static class Handler extends SimpleChannelInboundHandler<Push.PushMsg> {
    private final StatData mStatData;

    private final long mBeginTime;

    private boolean mLoggedIn = false;

    public Handler(StatData stat_data) {
      mBeginTime = System.currentTimeMillis();

      mStatData = stat_data;

      // ++
      mStatData.createdConnections.incrementAndGet();
      mStatData.waitingConnections.incrementAndGet();
      mStatData.createUsedTime.set(mBeginTime - mStatData.initialTime.get());

      printCount(mStatData);

    }

    private void login(ChannelHandlerContext ctx) {
      Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
      builder.setVersion(1);
      builder.setType(Push.MsgType.CLIENT_LOGIN);
      builder.setSequence(1);

      Push.ClientLogin.Builder login = Push.ClientLogin.newBuilder();
      login.setAccount("ztest@gmail.com");
      login.setPassword("123");

      builder.setClientLogin(login);

      ctx.writeAndFlush(builder.build());
    }

    private void printCount(StatData stat_data) {
      System.out.println(stat_data.toString());
    }

    @Override
    protected void channelRead0(final ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
      // do nothing
      if (!mLoggedIn) {
        if (msg.getType() != Push.MsgType.CLIENT_LOGIN_RSP && !msg.hasClientLoginRsp()) {
          System.out.println("unknown message");
          ctx.close();
          return;
        }

        Push.ClientLoginRsp rsp = msg.getClientLoginRsp();
        if (rsp.getCode() != 0) {
          System.out.println("failed response");
          ctx.close();
          return;
        }
        mLoggedIn = true;

        ctx.executor().schedule(new Runnable() {
          @Override
          public void run() {
            getFgwList(ctx);
          }
        }, 1, TimeUnit.SECONDS);
      } else {
        ctx.executor().schedule(new Runnable() {
          @Override
          public void run() {
            getFgwList(ctx);
          }
        }, 1, TimeUnit.SECONDS);
      }
    }

    private void getFgwList(ChannelHandlerContext ctx) {
      Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
      builder.setVersion(1);
      builder.setType(Push.MsgType.GET_FGW_LIST);
      builder.setSequence(2);

      Push.GetFgwList.Builder req = Push.GetFgwList.newBuilder();

      builder.setGetFgwList(req);

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
