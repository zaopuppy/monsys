package com.letsmidi.monsys.center;

import java.util.concurrent.ConcurrentHashMap;

import io.netty.channel.ChannelFuture;
import io.netty.util.concurrent.Future;

/**
 * Created by zero on 9/17/14.
 */
public class ServiceManager {

    public static interface Service {
        void send(Object msg);


        Future<Boolean> submit(Runnable task);

        ChannelFuture write(Object msg);

        ChannelFuture writeAndFlush(Object msg);

        Future<ChannelFuture> writeInEventLoop(Object msg);

        Future<ChannelFuture> writeAndFlushInEventLoop(Object msg);
    }

    // TODO: move it to a new file
    //public static class ChannelService implements Service {
    //  private final Channel mChannel;
    //
    //  public ChannelService(Channel channel) {
    //    mChannel = channel;
    //  }
    //
    //  @Override
    //  public void send(InnerAddress address, Object msg) {
    //    // convert from 1 message type to another
    //    String str = (String)msg;
    //
    //    // then
    //    Session session = new Session();
    //    session.setSourceAddress(address);
    //
    //    writeAndFlush(str);
    //
    //    save(session);
    //  }
    //
    //  @Override
    //  public Future<Boolean> submit(Runnable task) {
    //    // TODO: what should we do if channel is not active
    //    return mChannel.eventLoop().submit(task, true);
    //  }
    //
    //  @Override
    //  public ChannelFuture write(Object msg) {
    //    return mChannel.write(msg);
    //  }
    //
    //  @Override
    //  public ChannelFuture writeAndFlush(Object msg) {
    //    return mChannel.writeAndFlush(msg);
    //  }
    //
    //  @Override
    //  public Future<ChannelFuture> writeInEventLoop(final Object msg) {
    //    return mChannel.eventLoop().submit(new Callable<ChannelFuture>() {
    //      @Override
    //      public ChannelFuture call() throws Exception {
    //        return mChannel.write(msg);
    //      }
    //    });
    //  }
    //
    //  @Override
    //  public Future<ChannelFuture> writeAndFlushInEventLoop(final Object msg) {
    //    return mChannel.eventLoop().submit(new Callable<ChannelFuture>() {
    //      @Override
    //      public ChannelFuture call() throws Exception {
    //        return mChannel.writeAndFlush(msg);
    //      }
    //    });
    //  }
    //}

    public static final ServiceManager INSTANCE = new ServiceManager();

    private final ConcurrentHashMap<String, Service> mServiceMap = new ConcurrentHashMap<>(8);

    private ServiceManager() {
    }

    public Service register(String name, Service service) {
        return mServiceMap.put(name, service);
    }

    public Service get(String name) {
        return mServiceMap.getOrDefault(name, null);
    }

}
