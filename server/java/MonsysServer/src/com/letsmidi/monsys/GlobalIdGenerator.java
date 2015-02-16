package com.letsmidi.monsys;

import com.letsmidi.monsys.util.SequenceGenerator;

public class GlobalIdGenerator {
    public static final GlobalIdGenerator INSTANCE = new GlobalIdGenerator();

    private SequenceGenerator mGenerator = new SequenceGenerator(0, 0xFFFFFF);

    // disable constructor
    private GlobalIdGenerator() {
    }

    public int next() {
        return mGenerator.next();
    }
}
