package com.letsmidi.monsys.util;

/**
 * Created by zero on 8/16/14.
 */
public class MonsysException extends Exception {
  public MonsysException() {
    super();
  }

  public MonsysException(String message) {
    super(message);
  }

  public MonsysException(String message, Throwable cause) {
    super(message, cause);
  }

  public MonsysException(Throwable cause) {
    super(cause);
  }
}
