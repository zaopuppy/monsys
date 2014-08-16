package com.letsmidi.monsys.test;


import com.letsmidi.monsys.log.MyLogFormatter;
import com.letsmidi.monsys.util.BeanLoader;
import com.letsmidi.monsys.util.MonsysException;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.logging.*;

/**
 * For API testing
 *
 * Created by zero on 8/12/14.
 */
public class Test {
  public static class TestBean {
    private String mAbc;
    private int mIbc;
    public void setAbc(String abc) { mAbc = abc; }
    public String getAbc() { return mAbc; }
    public void setIbc(int ibc) { mIbc = ibc; }
    public int getIbc() { return mIbc; }
  }

  public static void main(String[] args) throws IOException, MonsysException {
    //TestBean b = new TestBean();
    TestBean test = BeanLoader.load(new File("test.cfg"), TestBean.class);
    System.out.println("abc: " + test.getAbc());
    System.out.println("ibc: " + test.getIbc());
  }

  private void testLogger() throws IOException {
    Logger logger = Logger.getLogger("");

    FileHandler log_handler = new FileHandler("test.log");
    log_handler.setFormatter(new MyLogFormatter());

    logger.addHandler(log_handler);
    logger.severe("very good");

    Logger child_logger = Logger.getLogger(Test.class.getCanonicalName());
    child_logger.severe("Child's good");
  }
}
