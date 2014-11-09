package com.letsmidi.monsys.protocol;

import android.util.Log;
import android.util.Pair;
import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.push.Push;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.util.concurrent.Future;

import java.net.InetSocketAddress;
import java.util.List;

// TODO: mChannel is null if network disconnected
public class MonsysServer {
  private static final String TAG = "XXX";

  public static final MonsysServer INSTANCE = new MonsysServer(Config.SERVER_HOST, 1988);

  private String mServerHost;
  private final int mServerPort;

  //private volatile InetSocketAddress mServerAddr = null;

  private NioEventLoopGroup mWorkerGroup = null;
  private Channel mChannel = null;

  private volatile Listener mListener = null;

  enum State {
    STATE_INIT,
    STATE_CONNECTING,
    STATE_CONNECTED,
    //STATE_LOGGED_IN,
  }

  private State mState = State.STATE_INIT;

  private void setState(State state) {
    mState = state;
  }

  private State getState() {
    return mState;
  }

  public boolean isConnected() { return mState == State.STATE_CONNECTED; }

  public static interface Listener {
    void onConnected();
    void onDisconnected();
    //void onLoggedIn();
    void onMessage(Push.PushMsg msg);
    void onException(Throwable cause);
  }

  public void setListener(Listener listener) {
    mListener = listener;
  }

  private class MsgHandler extends SimpleChannelInboundHandler<Push.PushMsg> {
    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
      Log.i(TAG, "channel closed");
      setState(State.STATE_INIT);
      super.channelInactive(ctx);
      // XXX
      // close();
      if (mListener != null) {
        mListener.onDisconnected();
      }
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
      Log.i(TAG, "channelActive");
      setState(State.STATE_CONNECTED);
      super.channelActive(ctx);
      if (mListener != null) {
        mListener.onConnected();
      }
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
      Log.i(TAG, "exception");
      cause.printStackTrace();
      super.exceptionCaught(ctx, cause);
      if (mListener != null) {
        mListener.onException(cause);
      }
      close();
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, Push.PushMsg msg) throws Exception {
      Log.d(TAG, "Received: " + msg.getType());
      //if (msg.getType() == Push.MsgType.CLIENT_LOGIN_RSP) {
      //  if (!msg.hasClientLoginRsp()) {
      //    Log.e(TAG, "bad response, close");
      //    close();
      //    return;
      //  }
      //  Push.ClientLoginRsp rsp = msg.getClientLoginRsp();
      //  if (rsp.getCode() != 0) {
      //    Log.e(TAG, "bad response: " + rsp.getCode());
      //    close();
      //    return;
      //  }
      //  setState(State.STATE_LOGGED_IN);
      //  if (mListener != null) {
      //    mListener.onLoggedIn();
      //  }
      //} else {
        if (mListener != null) {
          mListener.onMessage(msg);
        }
      //}
    }
  }

  public MonsysServer(String host, int port) {
    mServerHost = host;
    mServerPort = port;
  }

  public void close() {
    if (mWorkerGroup != null) {
      // XXX
      Future<?> future = mWorkerGroup.shutdownGracefully();
      // future.addListener()

      mWorkerGroup = null;
    }

    if (mChannel != null) {
      mChannel = null;
    }

    mState = State.STATE_INIT;
  }

  private class ConnectTask implements Runnable {
    //private final String mAccount;
    //private final String mPassword;
    public ConnectTask(/*String account, String password*/) {
      //mAccount = account;
      //mPassword = password;
    }

    @Override
    public void run() {
      InetSocketAddress addr = new InetSocketAddress(mServerHost, mServerPort);

      // don't cache it, server ip may change
      //mServerHost = addr.getAddress().getHostAddress();
      //mServerAddr = addr;

      close();

      Bootstrap b = new Bootstrap();

      mWorkerGroup = new NioEventLoopGroup(1);

      b.group(mWorkerGroup)
          .channel(NioSocketChannel.class)
          .handler(new ChannelInitializer<SocketChannel>() {
            @Override
            protected void initChannel(SocketChannel ch) throws Exception {
              ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                  new ProtobufVarint32FrameDecoder(),
                  new ProtobufEncoder(),
                  new ProtobufDecoder(Push.PushMsg.getDefaultInstance()),
                  new MsgHandler());
            }
          })
          .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 10 * 1000)
          .option(ChannelOption.SO_KEEPALIVE, true);

      // XXX: `onConnect' maybe triggered in this line, mChannel will still be null
      ChannelFuture future = b.connect(mServerHost, mServerPort);
      mChannel = future.channel();

      future.addListener(new ChannelFutureListener() {
        @Override
        public void operationComplete(ChannelFuture future) throws Exception {
          Log.i(TAG, "operation completed");
          if (future.isCancelled() || !future.isSuccess()) {
            // setState(State.STATE_INIT);
            close();
            if (mListener != null) {
              mListener.onDisconnected();
            }
          } else {
            // connectToFgw(device_id);
            //clientLogin(mAccount, mPassword);
            //if (mListener != null) {
            //  mListener.onConnected();
            //}
          }
        }
      });
    }
  }

  public void connect(/*final String account, final String password*/) {
    //ConnectTask task = new ConnectTask(account, password);
    ConnectTask task = new ConnectTask();
    //if (mServerAddr == null) {
    new Thread(task).start();
    //} else {
    //  task.run();
    //}
  }

  private Push.PushMsg.Builder newPushMsgBuilder(Push.MsgType type) {
    Push.PushMsg.Builder builder = Push.PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(type);
    builder.setSequence(4);
    return builder;
  }

  public void userRegister(String account, String password) {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Log.i(TAG, "userRegister()");

    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.USER_REGISTER);

    Push.UserRegister.Builder user_register = Push.UserRegister.newBuilder();
    user_register.setAccount(account);
    user_register.setNickname(account);
    user_register.setPassword(password);

    builder.setUserRegister(user_register);

    mChannel.writeAndFlush(builder.build());
  }

  public void clientLogin(String account, String password) {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Log.i(TAG, String.format("clientLogin(%s, %s)", account, password));
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.CLIENT_LOGIN);

    Push.ClientLogin.Builder login = Push.ClientLogin.newBuilder();
    login.setAccount(account);
    login.setPassword(password);

    builder.setClientLogin(login);

    mChannel.writeAndFlush(builder.build());
  }

  public void connectToFgw(String device_id) {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Log.i(TAG, "connectToFgw");
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.CONNECT);

    Push.Connect.Builder connect_req = Push.Connect.newBuilder();
    connect_req.setDeviceId(device_id);

    builder.setConnect(connect_req);

    mChannel.writeAndFlush(builder.build());
  }

  public void getFgwList() {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.GET_FGW_LIST);

    Push.GetFgwList.Builder get_fgw_list = Push.GetFgwList.newBuilder();

    builder.setGetFgwList(get_fgw_list);

    mChannel.writeAndFlush(builder.build());
  }

  public void getDevList() {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.GET_DEV_LIST);

    Push.GetDevList.Builder get_dev_list = Push.GetDevList.newBuilder();
    get_dev_list.setDeviceId("DEVID-Z");

    builder.setGetDevList(get_dev_list);

    mChannel.writeAndFlush(builder.build());
  }

  public void getDevInfo(int addr, List<Integer> id_list) {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.GET_DEV_INFO);

    Push.GetDevInfo.Builder get_dev_info = Push.GetDevInfo.newBuilder();
    get_dev_info.setAddr(addr);
    for (Integer id: id_list) {
      get_dev_info.addItemIds(id);
    }

    builder.setGetDevInfo(get_dev_info);

    mChannel.writeAndFlush(builder.build());
  }

  public void setDevInfo(int addr, List<Pair<Integer, Integer>> id_vals) {
    if (mChannel == null) {
      Log.e(TAG, "Not connected yet");
      return;
    }
    Push.PushMsg.Builder builder = newPushMsgBuilder(Push.MsgType.SET_DEV_INFO);

    Push.SetDevInfo.Builder set_dev_info = Push.SetDevInfo.newBuilder();
    set_dev_info.setAddr(addr);
    Push.IdValuePair.Builder id_val_pair = Push.IdValuePair.newBuilder();
    id_val_pair.setId(1);
    id_val_pair.setValue(44);
    set_dev_info.addIdValuePairs(id_val_pair);

    builder.setSetDevInfo(set_dev_info);

    mChannel.writeAndFlush(builder.build());
  }

  public ChannelFuture closeFuture() {
    if (mChannel == null) {
      return null;
    }

    return mChannel.closeFuture();
  }

}
