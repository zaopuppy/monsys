package com.letsmidi.monsys.util;

/**
 * Created by zero on 8/29/14.
 */
public class SequenceGenerator {
  private int mLast;
  private final int mMin;
  private final int mMax;

  public SequenceGenerator(int min, int max) {
    mMin = min;
    mMax = max;
    mLast = min;
  }

  public int next() {
    if (mLast > mMax) {
      mLast = mMin;
    }

    return mLast++;
  }
}
