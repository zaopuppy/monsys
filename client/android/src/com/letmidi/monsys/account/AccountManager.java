package com.letmidi.monsys.account;

import com.letmidi.monsys.protocol.MonsysInterface;

/**
 *
 * @author zero
 *
 */
public class AccountManager {
  private static final String TAG = "AccountManager";

  public void login(String account, String password) {
    MonsysInterface.login(account, password);
  }

  public void hasLoggedIn() {
  }

  public void logout() {
  }
}
