package com.letmidi.monsys.util;


public class StringUtil {
  // 这里的代码有没有办法不要重复啊.... 好郁闷
  public static String join(String sperator, String[] strings) {
    StringBuilder builder = new StringBuilder(512);
    boolean first = true;
    for (String s: strings) {
      if (first) {
        first = false;
      } else {
        builder.append(sperator);
      }
      builder.append(s);
    }
    return builder.toString();
  }

  public static String join(String sperator, Iterable<String> strings) {
    StringBuilder builder = new StringBuilder(512);
    boolean first = true;
    for (String s: strings) {
      if (first) {
        first = false;
      } else {
        builder.append(sperator);
      }
      builder.append(s);
    }
    return builder.toString();
  }
}
