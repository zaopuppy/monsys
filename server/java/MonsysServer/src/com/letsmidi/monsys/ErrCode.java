package com.letsmidi.monsys;

/**
 * Created by zero on 8/10/14.
 */
public enum ErrCode {
    FAIL(-1),
    SUCCESS(0),;

    private final int mValue;

    ErrCode(int v) {
        mValue = v;
    }

    public int value() {
        return mValue;
    }
}
