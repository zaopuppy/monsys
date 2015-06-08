package com.letsmidi.monsys;

import java.io.File;

import com.letsmidi.monsys.util.BeanLoader;
import com.letsmidi.monsys.util.MonsysException;

/**
 * Configuration for Super Router
 * <p>
 * Created by zero on 8/16/14.
 */
public class Config {

    private static final String CONFIG_FILE = "conf/common.cfg";
    private static final String PUSH_CONFIG_FILE = "conf/push.cfg";
    private static final String ROUTER_CONFIG_FILE = "conf/router.cfg";
    private static final String CENTER_CONFIG_FILE = "conf/center.cfg";

    public static class CommonConfig {
        private String zookeeperConnectString = null;
        private int zookeeperConnectTimeout = -1;
        private String hostName = null;
        private String zookeeperNameServicePath = null;

        public String getZookeeperConnectString() {
            return zookeeperConnectString;
        }

        public void setZookeeperConnectString(String zookeeperConnectString) {
            this.zookeeperConnectString = zookeeperConnectString;
        }

        public int getZookeeperConnectTimeout() {
            return zookeeperConnectTimeout;
        }

        public void setZookeeperConnectTimeout(int zookeeperConnectTimeout) {
            this.zookeeperConnectTimeout = zookeeperConnectTimeout;
        }

        public String getHostName() {
            return hostName;
        }

        public void setHostName(String hostName) {
            this.hostName = hostName;
        }

        public String getZookeeperNameServicePath() {
            return zookeeperNameServicePath;
        }

        public void setZookeeperNameServicePath(String zookeeperNameServicePath) {
            this.zookeeperNameServicePath = zookeeperNameServicePath;
        }
    }

    public static class RouterConfig {
        private int allocatePort = -1;
        private int accessPort = -1;
        private String logFileName = null;
        private String loggerName = null;
        private String nameServicePath = null;
        private String nameServiceNode = null;

        public int getAllocatePort() {
            return allocatePort;
        }

        public void setAllocatePort(int allocatePort) {
            this.allocatePort = allocatePort;
        }

        public int getAccessPort() {
            return accessPort;
        }

        public void setAccessPort(int accessPort) {
            this.accessPort = accessPort;
        }

        public String getLogFileName() {
            return logFileName;
        }

        public void setLogFileName(String logFileName) {
            this.logFileName = logFileName;
        }

        public String getLoggerName() {
            return loggerName;
        }

        public void setLoggerName(String loggerName) {
            this.loggerName = loggerName;
        }

        public String getNameServiceNode() {
            return nameServiceNode;
        }

        public void setNameServiceNode(String nameServiceNode) {
            this.nameServiceNode = nameServiceNode;
        }

        public String getNameServicePath() {
            return nameServicePath;
        }

        public void setNameServicePath(String nameServicePath) {
            this.nameServicePath = nameServicePath;
        }
    }

    public static class PushConfig {
        private String loggerName = null;
        private String logFileName = null;
        private int pushPort = -1;
        private int apiPort = -1;

        public String getLoggerName() {
            return loggerName;
        }

        public void setLoggerName(String loggerName) {
            this.loggerName = loggerName;
        }

        public String getLogFileName() {
            return logFileName;
        }

        public void setLogFileName(String logFileName) {
            this.logFileName = logFileName;
        }

        public int getPushPort() {
            return pushPort;
        }

        public void setPushPort(int pushPort) {
            this.pushPort = pushPort;
        }

        public int getApiPort() {
            return apiPort;
        }

        public void setApiPort(int apiPort) {
            this.apiPort = apiPort;
        }
    }

    public static class CenterConfig {
        private String loggerName = null;
        private String logFileName = null;
        private int accessPort = -1;
        private String nameServicePath = null;
        private String nameServiceNode = null;
        private String pushServerHost = null;
        private int pushServerPort = -1;
        private String routerServerHost = null;
        private int routerServerPort = -1;
        private int timeout = -1;

        public String getLoggerName() {
            return loggerName;
        }

        public void setLoggerName(String loggerName) {
            this.loggerName = loggerName;
        }

        public String getLogFileName() {
            return logFileName;
        }

        public void setLogFileName(String logFileName) {
            this.logFileName = logFileName;
        }

        public int getAccessPort() {
            return accessPort;
        }

        public void setAccessPort(int accessPort) {
            this.accessPort = accessPort;
        }

        public String getNameServiceNode() {
            return nameServiceNode;
        }

        public void setNameServiceNode(String nameServiceNode) {
            this.nameServiceNode = nameServiceNode;
        }

        public String getNameServicePath() {
            return nameServicePath;
        }

        public void setNameServicePath(String nameServicePath) {
            this.nameServicePath = nameServicePath;
        }

        public String getPushServerHost() {
            return pushServerHost;
        }

        public void setPushServerHost(String pushServerHost) {
            this.pushServerHost = pushServerHost;
        }

        public int getPushServerPort() {
            return pushServerPort;
        }

        public void setPushServerPort(int pushServerPort) {
            this.pushServerPort = pushServerPort;
        }

        public String getRouterServerHost() {
            return routerServerHost;
        }

        public void setRouterServerHost(String routerServerHost) {
            this.routerServerHost = routerServerHost;
        }

        public int getRouterServerPort() {
            return routerServerPort;
        }

        public void setRouterServerPort(int routerServerPort) {
            this.routerServerPort = routerServerPort;
        }

        public int getTimeout() {
            return timeout;
        }

        public void setTimeout(int timeout) {
            this.timeout = timeout;
        }
    }

    private static CommonConfig sCommonConfig = null;
    private static RouterConfig sRouterConfig = null;
    private static PushConfig sPushConfig = null;
    private static CenterConfig sCenterConfig = null;

    //
    //public static final String ZOOKEEPER_CONN_STR = "192.168.2.105:2181";
    //public static final int ZOOKEEPER_CONNECT_TIMEOUT = 10*1000;

    //
    //public static final String HOST_NAME = "letsmidi.wicp.net";
    //public static final int ACCESS_PORT = 4444;
    //public static final int ALLOCATE_PORT = 4445;

    // log
    //public static final String LOG_FILE_NAME = "super_router.log";
    //public static final String LOGGER_NAME = "super-router";
    //public static final String NS_PATH = "/services";
    //public static final String NS_NODE = "super-router";

    public static void load() throws MonsysException {
        sCommonConfig = BeanLoader.load(new File(CONFIG_FILE), CommonConfig.class);
        sRouterConfig = BeanLoader.load(new File(ROUTER_CONFIG_FILE), RouterConfig.class);
        sPushConfig = BeanLoader.load(new File(PUSH_CONFIG_FILE), PushConfig.class);
        sCenterConfig = BeanLoader.load(new File(CENTER_CONFIG_FILE), CenterConfig.class);
    }

    public static CommonConfig getCommonConfig() {
        return sCommonConfig;
    }

    public static RouterConfig getRouterConfig() {
        return sRouterConfig;
    }

    public static PushConfig getPushConfig() {
        return sPushConfig;
    }

    public static CenterConfig getCenterConfig() {
        return sCenterConfig;
    }
}

