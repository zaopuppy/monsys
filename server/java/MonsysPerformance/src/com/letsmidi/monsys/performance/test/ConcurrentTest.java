package com.letsmidi.monsys.performance.test;

import com.letsmidi.monsys.protocol.push.Push;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.function.IntBinaryOperator;

/**
 *
 * - Created Connections
 * - Established Connections
 * - Disconnected Connections
 * - Waiting Connections
 * - Maximum Delay (in milliseconds)
 * - Minimum Delay (in milliseconds)
 * - Average Delay (in milliseconds)
 * - Spawn Rate (cps = connections per second)
 * - Total Time Cost (util all connection established)
 *
 *
 * Created by zero on 9/4/14.
 */
public abstract class ConcurrentTest extends Test {

  public static class StatData {
    public AtomicInteger createdConnections = new AtomicInteger(0);
    public AtomicInteger establishedConnections = new AtomicInteger(0);
    public AtomicInteger disconnectedConnections = new AtomicInteger(0);
    public AtomicInteger waitingConnections = new AtomicInteger(0);

    public AtomicInteger totalDelay = new AtomicInteger(0);
    public AtomicInteger maxDelay = new AtomicInteger(0);
    public AtomicInteger minDelay = new AtomicInteger(0xFFFFFF);

    public AtomicLong initialTime = new AtomicLong(0);
    public AtomicLong createUsedTime = new AtomicLong(0);

    public void addDelay(int delay) {
      totalDelay.accumulateAndGet(delay, new IntBinaryOperator() {
        @Override
        public int applyAsInt(int left, int right) {
          return left + right;
        }
      });

      maxDelay.accumulateAndGet(delay, new IntBinaryOperator() {
        @Override
        public int applyAsInt(int left, int right) {
          if (right > left) {
            return right;
          } else {
            return left;
          }
        }
      });

      minDelay.accumulateAndGet(delay, new IntBinaryOperator() {
        @Override
        public int applyAsInt(int left, int right) {
          if (right < left) {
            return right;
          } else {
            return left;
          }
        }
      });
    }

    public int getAverageDelay() {
      int total_delay = this.totalDelay.get();
      int created_count = this.createdConnections.get();

      assert(created_count > 0);

      return total_delay / created_count;
    }

    public float getCreateRate() {
      long create_used_time = this.createUsedTime.get();
      int created_count = this.createdConnections.get();

      return (float)create_used_time / (float)created_count * 1000;
    }

    public float getEstablishRate() {
      long create_used_time = this.createUsedTime.get();
      int established_count = this.establishedConnections.get();

      return (float)create_used_time / (float)established_count * 1000;
    }

    @Override
    public String toString() {
      return String.format(
          "conn(%d %d %d %d) delay(%d %d %d) time(%d) rate(%f, %f)",
          createdConnections.get(),
          establishedConnections.get(),
          disconnectedConnections.get(),
          waitingConnections.get(),

          maxDelay.get(),
          minDelay.get(),
          getAverageDelay(),

          createUsedTime.get(),

          getCreateRate(),
          getEstablishRate()
          );
    }
  }

  private final String mServerHost;
  private final int mServerPort;
  private final int mConnectionCount;

  public ConcurrentTest(String host, int port, int connect_count) {
    mServerHost = host;
    mServerPort = port;
    mConnectionCount = connect_count;
  }

  public abstract ChannelHandler newHandler();

  @Override
  public void start() {

    NioEventLoopGroup worker = new NioEventLoopGroup();

    try {
      Bootstrap b = new Bootstrap();

      b.group(worker)
          .channel(NioSocketChannel.class)
          .handler(new ChannelInitializer<SocketChannel>() {
            @Override
            protected void initChannel(SocketChannel ch) throws Exception {
              ch.pipeline().addLast(new ProtobufVarint32LengthFieldPrepender(),
                  new ProtobufVarint32FrameDecoder(),
                  new ProtobufEncoder(),
                  new ProtobufDecoder(Push.PushMsg.getDefaultInstance()),
                  newHandler());
            }
          })
          .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 60 * 1000)
          .option(ChannelOption.SO_KEEPALIVE, true);

      ChannelFuture f = null;
      for (int i = 0; i < mConnectionCount; ++i) {
        f = b.connect(mServerHost, mServerPort);
        Thread.sleep(50);
      }

      if (f != null) {
        f.channel().closeFuture().sync();
      }

    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      worker.shutdownGracefully();
    }
  }

  @Override
  public void printResult() {
    System.out.println("printResult()");
  }
}
