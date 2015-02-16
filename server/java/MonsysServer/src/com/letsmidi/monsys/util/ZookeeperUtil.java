package com.letsmidi.monsys.util;

import java.io.IOException;
import java.util.List;
import java.util.logging.Logger;

import com.letsmidi.monsys.Config;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;

/**
 * Created by zero on 8/16/14.
 */
public class ZookeeperUtil {
    private static ZooKeeper mKeeper = null;

    private static final Logger mLogger = Logger.getLogger(Config.getRouterConfig().getLoggerName());

    public static ZooKeeper getKeeper() {
        return mKeeper;
    }

    public static boolean init(String conn_str, int timeout) {
        mLogger.info("init()");

        ZooKeeper keeper = null;

        try {
            keeper = new ZooKeeper(
                    conn_str, timeout,
                    new Watcher() {
                        @Override
                        public void process(WatchedEvent event) {
                            //
                        }
                    },
                    true
            );

            mKeeper = keeper;

            mLogger.info("initialized");

            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static void close() {
        if (mKeeper != null) {
            try {
                mKeeper.close();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public static boolean register(String path, String node, byte[] data) {
        //if (!create_r(path, Config.HOST_NAME.getBytes(), ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT)) {
        if (!create_r(path, "monsys-server".getBytes(), ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT)) {
            return false;
        }

        if (!create(path + "/" + node, data, ZooDefs.Ids.READ_ACL_UNSAFE, CreateMode.EPHEMERAL_SEQUENTIAL)) {
            return false;
        }

        return true;
    }

    private static boolean create(String path, byte[] data, List<ACL> acls, CreateMode mode) {
        mLogger.info("create(" + path + ")");

        assert (mKeeper != null);

        boolean result = false;

        try {
            mKeeper.create(path, data, acls, mode);
            result = true;
        } catch (KeeperException.NodeExistsException e) {
            mLogger.info(e.toString());
            result = true;
        } catch (KeeperException | InterruptedException e) {
            e.printStackTrace();
        }

        return result;
    }

    private static boolean create_r(String path, byte[] data, List<ACL> acls, CreateMode mode) {
        mLogger.info("create_r(" + path + ")");

        assert (mKeeper != null);

        String subpath;
        for (int idx = path.indexOf('/', 1); idx >= 0; idx = path.indexOf('/', idx + 1)) {
            subpath = path.substring(0, idx);
            if (!create(subpath, data, acls, mode)) {
                mLogger.severe("Failed to create path: [" + subpath + "]");
                return false;
            }
        }

        return create(path, data, acls, mode);
    }
}
