package com.letsmidi.monsys.push;

import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.util.internal.PlatformDependent;

public class FgwManager {
    // Singleton
    public static FgwManager INSTANCE = new FgwManager();

    private final Logger mLogger = Logger.getLogger(Config.getPushConfig().getLoggerName());

    public static class FgwInfo {
        public final Channel channel;
        public AtomicBoolean connected = new AtomicBoolean(false);
        public final String deviceId;

        public FgwInfo(String deviceId, Channel ch) {
            this.deviceId = deviceId;
            this.channel = ch;
        }
    }

    private class ClientInfoRemover implements ChannelFutureListener {
        private final String mDeviceId;

        public ClientInfoRemover(String devId) {
            mDeviceId = devId;
        }

        @Override
        public void operationComplete(ChannelFuture future) throws Exception {
            mClientMap.remove(mDeviceId);
        }
    }

    private final ConcurrentMap<String, FgwInfo> mClientMap = PlatformDependent.newConcurrentHashMap(1000);

    private FgwManager() {
        // TODO
    }

    public boolean add(FgwInfo info) {
        // return mChannelGroup.add(ch);
        FgwInfo old = mClientMap.putIfAbsent(info.deviceId, info);
        if (old != null) {
            mLogger.severe("Duplicated client");
            return false;
        }

        // XXX: not good, new listener everything...
        info.channel.closeFuture().addListener(new ClientInfoRemover(info.deviceId));

        return true;
    }

    public FgwInfo find(String device_id) {
        return mClientMap.get(device_id);
    }
}

