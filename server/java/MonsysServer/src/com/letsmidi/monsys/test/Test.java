package com.letsmidi.monsys.test;


import com.letsmidi.monsys.log.MyLogFormatter;

import java.io.IOException;
import java.util.logging.*;

/**
 * For API testing
 *
 * Created by zero on 8/12/14.
 */
public class Test {
  public static void main(String[] args) throws IOException {
    Logger logger = Logger.getLogger("");

    FileHandler log_handler = new FileHandler("test.log");
    log_handler.setFormatter(new MyLogFormatter());

    logger.addHandler(log_handler);
    logger.severe("very good");

    Logger child_logger = Logger.getLogger(Test.class.getCanonicalName());
    child_logger.severe("Child's good");
  }
}
