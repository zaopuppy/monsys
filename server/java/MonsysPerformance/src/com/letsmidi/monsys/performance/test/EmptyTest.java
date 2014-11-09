package com.letsmidi.monsys.performance.test;

/**
 * Created by zero on 9/4/14.
 */
public class EmptyTest extends Test {
  @Override
  public void start() {
    System.out.println("EmptyTest, do nothing");
  }

  @Override
  public void printResult() {
    System.out.println("Success");
  }
}
