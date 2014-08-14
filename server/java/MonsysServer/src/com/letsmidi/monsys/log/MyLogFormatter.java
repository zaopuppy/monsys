package com.letsmidi.monsys.log;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;
import java.util.logging.Formatter;
import java.util.logging.LogRecord;

/**
 *
 * Created by zero on 8/12/14.
 */
public class MyLogFormatter extends Formatter {

  private final Date mDate = new Date();

  @Override
  public String format(LogRecord record) {
    mDate.setTime(record.getMillis());
    String source;
    if (record.getSourceClassName() != null) {
      source = record.getSourceClassName();
      if (record.getSourceMethodName() != null) {
        source += " " + record.getSourceMethodName();
      }
    } else {
      source = record.getLoggerName();
    }
    String message = formatMessage(record);
    return String.format("%1$tF_%1$tT|%2$s|%3$s|%4$s\n",
        mDate,
        record.getLevel(),
        // record.getLoggerName(),
        source,
        message);
  }
}
