package com.letmidi.monsys.account;

import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.letmidi.monsys.DevInfo;
import com.letmidi.monsys.FgwInfo;
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

  private static boolean mIsLoggedIn = false;

  // login
  public interface LoginCallback {
    void onLogin(boolean result);
  }

  public static boolean login(String account, String password) {
    mIsLoggedIn = MonsysInterface.login(account, password);
    return mIsLoggedIn;
  }

  public static void loginAsync(final String account, final String password, final LoginCallback callback) {
    sExecutor.execute(new Runnable() {
      @Override
      public void run() {
        callback.onLogin(login(account, password));
      }
    });
  }

  // get-fgw-list
  public interface GetFgwListCallback {
    void onGetFgwList(List<FgwInfo> fgw_list);
  }

  public static List<FgwInfo> getFgwList(String account) {
    return MonsysInterface.getFgwList(account);
  }

  public static void getFgwListAsync(final String account, final GetFgwListCallback callback) {
    sExecutor.execute(new Runnable() {
      @Override
      public void run() {
        callback.onGetFgwList(getFgwList(account));
      }
    });
  }

  // get-dev-list
  public interface GetDevListCallback {
    void onGetDevList(List<DevInfo> dev_list);
  }

  public static List<DevInfo> getDevList(String fgw_id) {
    return MonsysInterface.getDevList(fgw_id);
  }

  public static void getDevListAsync(final String fgw_id, final GetDevListCallback callback) {
    sExecutor.execute(new Runnable() {
      @Override
      public void run() {
        callback.onGetDevList(MonsysInterface.getDevList(fgw_id));
      }
    });
  }

  //
  public static boolean isLoggedIn() {
    return mIsLoggedIn;
  }

  public static void logout() {
    mIsLoggedIn = false;
    MonsysInterface.logout();
  }
}
