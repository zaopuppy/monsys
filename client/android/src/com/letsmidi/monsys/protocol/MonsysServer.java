package com.letsmidi.monsys.protocol;

import io.netty.bootstrap.Bootstrap;
import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.SimpleChannelInboundHandler;
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

import android.util.Log;
import android.util.Pair;

import com.letsmidi.monsys.Config;
import com.letsmidi.monsys.protocol.Push.ClientLogin;
import com.letsmidi.monsys.protocol.Push.ClientLoginRsp;
import com.letsmidi.monsys.protocol.Push.Connect;
import com.letsmidi.monsys.protocol.Push.GetDevInfo;
import com.letsmidi.monsys.protocol.Push.GetDevList;
import com.letsmidi.monsys.protocol.Push.GetFgwList;
import com.letsmidi.monsys.protocol.Push.IdValuePair;
import com.letsmidi.monsys.protocol.Push.MsgType;
import com.letsmidi.monsys.protocol.Push.PushMsg;
import com.letsmidi.monsys.protocol.Push.SetDevInfo;

// TODO: mChannel is null if network disconnected
//
public class MonsysServer {
  private static final String TAG = "XXX";

  // letsmidi.wicp.net
  public static final MonsysServer INSTANCE = new MonsysServer(Config.SERVER_HOST, 1988);

  private String mServerHost;
  private final int mServerPort;

  private volatile InetSocketAddress mServerAddr = null;

  private NioEventLoopGroup mWorkerGroup = null;
  private Channel mChannel = null;

  private volatile Listener mListener = null;

  enum State {
    STATE_INIT,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_LOGGED_IN,
  }

  private State mState = State.STATE_INIT;

  private void setState(State state) {
    mState = state;
  }

  private State getState() {
    return mState;
  }

  public boolean isLoggedIn() {
    return getState() == State.STATE_LOGGED_IN;
  }

  public static interface Listener {
    void onConnected();
    void onDisconnected();
    void onLoggedIn();
    void onMessage(PushMsg msg);
    void onException(Throwable cause);
  }

  public void setListener(Listener listener) {
    mListener = listener;
  }

  private class MsgHandler extends SimpleChannelInboundHandler<PushMsg> {
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
      Log.i(TAG, "connected");
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
    protected void channelRead0(ChannelHandlerContext ctx, PushMsg msg) throws Exception {
      Log.d(TAG, "Received: " + msg.getType());
      if (msg.getType() == MsgType.CLIENT_LOGIN_RSP) {
        if (!msg.hasClientLoginRsp()) {
          Log.e(TAG, "bad response, close");
          close();
          return;
        }
        ClientLoginRsp rsp = msg.getClientLoginRsp();
        if (rsp.getCode() != 0) {
          Log.e(TAG, "bad response: " + rsp.getCode());
          close();
          return;
        }
        setState(State.STATE_LOGGED_IN);
        if (mListener != null) {
          mListener.onLoggedIn();
        }
      } else {
        if (mListener != null) {
          mListener.onMessage(msg);
        }
      }
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
    private final String mAccount;
    private final String mPassword;
    public ConnectTask(String account, String password) {
      mAccount = account;
      mPassword = password;
    }

    @Override
    public void run() {
      InetSocketAddress addr = new InetSocketAddress(mServerHost, mServerPort);

      mServerHost = addr.getAddress().getHostAddress();
      mServerAddr = addr;

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
                  new ProtobufDecoder(PushMsg.getDefaultInstance()),
                  new MsgHandler());
            }
          })
          .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 60 * 1000)
          .option(ChannelOption.SO_KEEPALIVE, true);

      // XXX: `onConnect' maybe triggered in this line, mChannel will still be null
      ChannelFuture future = b.connect(mServerHost, mServerPort);
      mChannel = future.channel();

      future.addListener(new ChannelFutureListener() {
        @Override
        public void operationComplete(ChannelFuture future) throws Exception {
          Log.i(TAG, "operation completed");
          if (future.isCancelled() || !future.isSuccess()) {
            setState(State.STATE_INIT);
          } else {
            // connectToFgw(device_id);
            clientLogin(mAccount, mPassword);
          }
        }
      });
    }
  }

  public void connect(final String account, final String password) {
    ConnectTask task = new ConnectTask(account, password);
    if (mServerAddr == null) {
      new Thread(task).start();
    } else {
      task.run();
    }
  }

  private PushMsg.Builder newPushMsgBuilder(MsgType type) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(type);
    return builder;
  }

  private void clientLogin(String account, String password) {
    Log.i(TAG, String.format("clientLogin(%s, %s)", account, password));
    PushMsg.Builder builder = newPushMsgBuilder(MsgType.CLIENT_LOGIN);

    ClientLogin.Builder login = ClientLogin.newBuilder();
    login.setAccount(account);
    login.setPassword(password);

    builder.setClientLogin(login);

    mChannel.writeAndFlush(builder.build());
  }

  public void connectToFgw(String device_id) {
    Log.i(TAG, "connectToFgw");
    PushMsg.Builder builder = newPushMsgBuilder(MsgType.CONNECT);

    Connect.Builder connect_req = Connect.newBuilder();
    connect_req.setDeviceId(device_id);

    builder.setConnect(connect_req);

    mChannel.writeAndFlush(builder.build());
  }

  public void getFgwList() {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.GET_FGW_LIST);

    GetFgwList.Builder get_fgw_list = GetFgwList.newBuilder();

    builder.setGetFgwList(get_fgw_list);

    mChannel.writeAndFlush(builder.build());
  }

  public void getDevList() {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.GET_DEV_LIST);

    GetDevList.Builder get_dev_list = GetDevList.newBuilder();
    get_dev_list.setDeviceId("DEVID-Z");

    builder.setGetDevList(get_dev_list);

    mChannel.writeAndFlush(builder.build());
  }

  public void getDevInfo(int addr, List<Integer> id_list) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.GET_DEV_INFO);

    GetDevInfo.Builder get_dev_info = GetDevInfo.newBuilder();
    get_dev_info.setAddr(addr);
    for (Integer id: id_list) {
      get_dev_info.addItemIds(id);
    }

    builder.setGetDevInfo(get_dev_info);

    mChannel.writeAndFlush(builder.build());
  }

  public void setDevInfo(int addr, List<Pair<Integer, Integer>> id_vals) {
    PushMsg.Builder builder = PushMsg.newBuilder();
    builder.setVersion(1);
    builder.setType(MsgType.SET_DEV_INFO);

    SetDevInfo.Builder set_dev_info = SetDevInfo.newBuilder();
    set_dev_info.setAddr(addr);
    IdValuePair.Builder id_val_pair = IdValuePair.newBuilder();
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

    /*
    public static void main(String[] args) {
        final MonsysServer server = new MonsysServer("localhost", 1988);

        server.connect();
//        server.connect(new ChannelFutureListener() {
//            @Override
//            public void operationComplete(ChannelFuture future) throws Exception {
//                System.out.println("   isDone: " + future.isDone());
//                System.out.println("isSuccess: " + future.isSuccess());
//                if (future.isSuccess()) {
//                    server.connectToFgw("DEVID-Z");
//                }
//            }
//        });
//
        // try {
        // ChannelFuture future = server.closeFuture();
        // if (future != null) {
        // future.wait();
        // }
        // } catch (InterruptedException e) {
        // e.printStackTrace();
        // }

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

        try {
            for (String line = in.readLine().trim(); line != null; line = in.readLine().trim()) {
                System.out.println("input: [" + line + "]");
                if (line.equals("exit")) {
                    break;
                }

                if (line.startsWith("list")) {
                    server.getDevList();
                } else if (line.startsWith("get")) {
                    // server.getDevInfo();
                } else if (line.startsWith("set")) {
                    // server.setDevInfo();
                } else {
                    System.out.println("Unknown command");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        server.close();
    }*/

}
