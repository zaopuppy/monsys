package com.letsmidi.monsys.log;

import java.text.SimpleDateFormat;
import java.util.Date;

public class Logger {

    private static final ThreadLocal<SimpleDateFormat> DATE_FORMATE = new ThreadLocal<SimpleDateFormat>() {

        @Override
        protected SimpleDateFormat initialValue() {
            return new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
        }

    };

    //private static Logger sLogger = Logger.getLogger("default");

    public static void main(String[] args) {
        traceBin("hello, world\r\n\0\0\0".getBytes());
    }

    //public static void setLogger(Logger logger) {
    //  sLogger = logger;
    //}

    private static String getDateStr() {
        return DATE_FORMATE.get().format(new Date());
    }

    public static void d(String tag, String msg) {
        //sLogger.info(msg);
        System.out.println(getDateStr() + "|" + msg);
    }

    public static void i(String tag, String msg) {
        //sLogger.fine(msg);
        System.out.println(getDateStr() + "|" + msg);
    }

    public static void w(String tag, String msg) {
        //sLogger.warning(msg);
        System.out.println(getDateStr() + "|" + msg);
    }

    public static void e(String tag, String msg) {
        //sLogger.severe(msg);
        System.out.println(getDateStr() + "|" + msg);
    }

    private static char[] sHexMap = "0123456789ABCDEF".toCharArray();

    public static void traceBin(byte[] bytes) {
        // 00000 00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF
        // ................\n
        final int LINE_LEN = 7 + 16 * 3 + 1 + 16 + 1;
        final int left_bytes = bytes.length & 0xF;
        int lines = bytes.length / 16;
        if (left_bytes > 0) {
            ++lines;
        }

        StringBuilder builder = new StringBuilder(LINE_LEN * lines);

        for (int i = 0; i < lines - 1; ++i) {
            builder.append(String.format("%05X  ", i));
            for (int j = 0; j < 16; ++j) {
                byte b = bytes[i * 16 + j];
                builder.append(sHexMap[(b >> 4) & 0xF]);
                builder.append(sHexMap[(b) & 0xF]);
                builder.append(' ');
            }
            builder.append(' ');
            for (int j = 0; j < 16; ++j) {
                builder.append(b2c(bytes[i * 16 + j]));
            }
            builder.append('\n');
        }

        if (left_bytes > 0) {
            builder.append(String.format("%05X  ", lines - 1));
            for (int j = 0; j < 16; ++j) {
                if (j < left_bytes) {
                    byte b = bytes[(lines - 1) * 16 + j];
                    builder.append(sHexMap[(b >> 4) & 0xF]);
                    builder.append(sHexMap[(b) & 0xF]);
                    builder.append(' ');
                } else {
                    builder.append(' ');
                    builder.append(' ');
                    builder.append(' ');
                }
            }
            builder.append(' ');
            for (int j = 0; j < 16; ++j) {
                if (j < left_bytes) {
                    builder.append(b2c(bytes[(lines - 1) * 16 + j]));
                } else {
                    builder.append(' ');
                }
            }
            builder.append('\n');
        }

        System.out.println(builder.toString());
    }

    private static char b2c(byte b) {
        if (b < ' ' || b > '~') {
            return '.';
        }

        return (char) b;
    }
}
