package com.letsmidi.monsys.performance;

import com.letsmidi.monsys.performance.test.*;

public class Main {
  public static void main(String[] args) {
    System.out.println("Performance tool started");

    if (args.length <= 0) {
      return;
    }

    String test_type = args[0];
    Test test = null;
    if ("client".equals(test_type)) {
      test = new ClientStubTest("192.168.2.105", 1988, 1000);
    } else if ("fgw".equals(test_type)) {
      test = new FgwStubTest("192.168.2.105", 1984, 1000);
    } else {
      // default
      test = new EmptyTest();
    }

    test.start();

    test.printResult();

    return;
  }
}
