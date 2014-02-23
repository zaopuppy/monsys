package com.letmidi.monsys.account;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.letmidi.monsys.protocol.MonsysInterface;

/**
 *
 * @author zero
 *
 */
public class AccountManager {
  private static final String TAG = "AccountManager";

  /**
   * for executing tasks in background
   * @author zero
   *
   */
  private static class MyThreadFactory implements ThreadFactory {
    private final AtomicInteger mCount = new AtomicInteger(1);

    @Override
    public Thread newThread(Runnable r) {
        return new Thread(r, "MonsysAsyncTask #" + mCount.getAndIncrement());
    }
  }

  private static final int CORE_POOL_SIZE = 2;
  private static final int MAX_POOL_SIZE = 5;
  private static final int THREAD_KEEP_ALIVE = 5;

  private static final BlockingQueue<Runnable> sWorkQueue =
      new LinkedBlockingQueue<Runnable>(16);
  private static final Executor sExecutor =
      new ThreadPoolExecutor(CORE_POOL_SIZE, MAX_POOL_SIZE, THREAD_KEEP_ALIVE,
                             TimeUnit.SECONDS, sWorkQueue, new MyThreadFactory());

  public interface LoginCallback {
    void onLogin(boolean result);
  }

  public static boolean login(String account, String password) {
    return MonsysInterface.login(account, password);
  }

  public static void loginAsync(final String account, final String password, final LoginCallback callback) {
    sExecutor.execute(new Runnable() {
      @Override
      public void run() {
        callback.onLogin(MonsysInterface.login(account, password));
      }
    });
  }

  public static void hasLoggedIn() {
  }

  public static void logout() {
  }
}
