package com.letsmidi.monsys.protocol;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;
import android.util.Pair;

import com.letsmidi.monsys.DevInfo;
import com.letsmidi.monsys.FgwInfo;

public class MonsysInterface {
  private static final String TAG = "XXX";
  private static final String SERVER_LOGIN_HOST =
      "http://letsmidi.wicp.net:1983/interface/login";
  private static final String SERVER_HOST =
      "http://letsmidi.wicp.net:1983/interface";
  // private static final String SERVER_LOGIN_HOST = "http://192.168.2.146:8888/interface/login";
  // private static final String SERVER_HOST = "http://192.168.2.146:8888/interface";
  private static String mCookies = null;

  public static class Response {
    public int status = 404;
    public String statusText = null;
    public String data = null;

    @Override
    public String toString() {
      return "Response [status=" + status + ", statusText=" + statusText + ", data=" + data + "]";
    }
  }

  public static void logout() {
    mCookies = null;
  }

  /**
   *
   * @param account
   * @param password
   * @return
   */
  public static boolean login(String account, String password) {
    // XXX: 安全問題
    HttpURLConnection conn = post(SERVER_LOGIN_HOST,
                                  "account=" + account + "&password=" + password);
    if (conn == null) {
      return false;
    }

    try {
      int status = conn.getResponseCode();
      String rsp = conn.getResponseMessage();

      Log.d(TAG, "received(" + status + "): " + rsp);

      if (status != HttpURLConnection.HTTP_OK) {
        return false;
      }

      if (!saveCookies(conn.getHeaderFields())) {
        return false;
      }

      return true;

    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (conn != null) {
        conn.disconnect();
      }
    }

    return false;
  }

  /**
   *
   * @param account
   * @return
   */
  public static List<FgwInfo> getFgwList(String account) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-fgw-list");
      jreq.put("account", account);
      jreq.put("uid", "456");
      jreq.put("seq", generateSequence());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return null;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return null;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return null;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      JSONArray jfgws = jrsp.getJSONArray("fgws");

      List<FgwInfo> fgw_list = new LinkedList<FgwInfo>();
      for (int i = 0; i < jfgws.length(); ++i) {
        String device_id = jfgws.getString(i);
        FgwInfo info = new FgwInfo();
        info.name = device_id;
        info.id = device_id;
        fgw_list.add(info);
      }

      return fgw_list;
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return null;
  }

  /**
   *
   * @param fgw_id
   * @return
   */
  public static List<DevInfo> getDevList(String fgw_id) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-dev-list");
      jreq.put("fgw", fgw_id);
      jreq.put("seq", generateSequence());
      jreq.put("uid", "456");
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return null;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return null;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return null;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      JSONArray jdevs = jrsp.getJSONArray("devs");

      List<DevInfo> dev_list = new LinkedList<DevInfo>();
      for (int i = 0; i < jdevs.length(); ++i) {
        JSONObject jobj = jdevs.getJSONObject(i);

        DevInfo info = new DevInfo();
        info.name = jobj.getString("name");
        info.addr = jobj.getInt("addr");
        info.type = jobj.getInt("type");
        dev_list.add(info);
      }

      return dev_list;
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return null;
  }

  /**
   *
   * @param fgw_id
   * @param dev_addr
   * @param ids
   * @return
   */
  public static List<Pair<Integer, Integer>> getDevInfo(String fgw_id, int dev_addr, List<Integer> ids) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-dev-info");
      jreq.put("fgw", fgw_id);
      jreq.put("addr", dev_addr);
      jreq.put("seq", generateSequence());
      jreq.put("uid", "456");
      JSONArray jid_list = new JSONArray(ids);
      jreq.put("id-list", jid_list);
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return null;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return null;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return null;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      JSONArray jid_infos = jrsp.getJSONArray("id-infos");

      List<Pair<Integer, Integer>> info_list = new LinkedList<Pair<Integer, Integer>>();
      for (int i = 0; i < jid_infos.length(); ++i) {
        JSONObject jobj = jid_infos.getJSONObject(i);

        info_list.add(
            new Pair<Integer, Integer>(
                jobj.getInt("id"), jobj.getInt("val")));
      }

      return info_list;
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return null;
  }

  /**
   *
   * @param fgw_id
   * @param dev_addr
   * @param ids
   * @return status
   *
   */
  public static int setDevInfo(String fgw_id, int dev_addr, List<Pair<Integer, Integer>> id_vals) {
    Log.d(TAG, "setDevInfo()");
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "set-dev-info");
      jreq.put("fgw", fgw_id);
      jreq.put("addr", dev_addr);
      jreq.put("seq", generateSequence());
      jreq.put("uid", "456");
      JSONArray jid_list = new JSONArray();
      for (Pair<Integer, Integer> id_val: id_vals) {
        JSONObject value = new JSONObject();
        value.put("id", id_val.first);
        value.put("val", id_val.second);
        jid_list.put(value);
      }
      jreq.put("vals", jid_list);
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return -1;
    }

    Log.d(TAG, "set-dev-info: [" + jreq.toString() + "]");

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return -1;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return -1;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      return jrsp.getInt("status");
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return -1;
  }

  public static boolean preBind(String fgw_id, String account) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "pre-bind");
      jreq.put("fgw", fgw_id);
      jreq.put("account", account);
      jreq.put("seq", generateSequence());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return false;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return false;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return false;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      int result = jrsp.getInt("result");
      if (result != 0) {
        return false;
      }

      return true;
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return false;
  }

  public static boolean bind(String fgw_id, String account) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "bind");
      jreq.put("fgw", fgw_id);
      jreq.put("account", account);
      jreq.put("seq", generateSequence());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return false;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return false;
    }

    Response rsp = getResponse(conn);
    if (rsp == null) {
      return false;
    }

    try {
      JSONObject jrsp = new JSONObject(rsp.data);
      int result = jrsp.getInt("result");
      if (result != 0) {
        return false;
      }

      return true;
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    }

    return false;
  }

  private static Response getResponse(HttpURLConnection conn) {
    InputStream in = null;
    try {
      Response rsp = new Response();

      rsp.status = conn.getResponseCode();
      rsp.statusText = conn.getResponseMessage();

      in = new BufferedInputStream(conn.getInputStream());

      byte[] buffer = new byte[1024];
      int rv = in.read(buffer);

      rsp.data = new String(buffer, 0, rv, "utf-8");

      Log.d(TAG, "response: " + rsp.toString());

      return rsp;
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      if (conn != null) {
        conn.disconnect();
      }
    }

    return null;
  }

  private static boolean saveCookies(Map<String, List<String>> headers) {
    // extract all cookies and fiter duplicated ones
    Map<String, String> key_map = new HashMap<String, String>(10);
    String[] fields;
    String[] kv;
    boolean has_cookie = false;
    for (Map.Entry<String, List<String>> entry : headers.entrySet()) {
      if ("Set-Cookie".equals(entry.getKey())) {
        has_cookie = true;
        // "account=enRlc3RAZ21haWwuY29t|1388901264|0930497a9d4b93d45026601af43a0191edf85bab; expires=Tue, 04 Feb 2014 05:54:24 GMT; Path=/"
        for (String value : entry.getValue()) {
          fields = value.split(";");
          for (String field : fields) {
            kv = field.trim().split("=");
            if (kv.length != 2) {
              Log.e(TAG, "!!! field = [" + field + "], split size: " + kv.length);
              continue;
            }
            key_map.put(kv[0], kv[1]);
          }
        }
      }
    }

    if (!has_cookie) {
      return false;
    }

    // join them~
    StringBuilder builder = new StringBuilder(512);
    boolean first = true;
    for (Map.Entry<String, String> entry : key_map.entrySet()) {
      if (first) {
        first = false;
      } else {
        builder.append("; ");
      }
      builder.append(entry.getKey());
      builder.append('=');
      builder.append(entry.getValue());
    }

    if (builder.length() > 0) {
      mCookies = builder.toString();
    }

    if (null == mCookies) {
      return false;
    }

    Log.d(TAG, "final cookies is: [" + mCookies + "]");

    return true;
  }

  private static HttpURLConnection getConnection(String host) {
    URL url;

    try {
      url = new URL(host);
    } catch (MalformedURLException e) {
      Log.e(TAG, "exception: " + e.toString());
      return null;
    }

    HttpURLConnection conn = null;

    try {
      conn = (HttpURLConnection) url.openConnection();
      conn.setConnectTimeout(5 * 1000);
      conn.setReadTimeout(2 * 1000);
      conn.setUseCaches(true);

      // FIXME
      // set cookies
      if (mCookies != null) {
        conn.setRequestProperty("Cookie", mCookies);
      }

      return conn;
    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (conn != null) {
        conn.disconnect();
      }
    }

    return null;
  }

  private static HttpURLConnection post(String host, String data) {
    HttpURLConnection conn = getConnection(host);
    if (conn == null) {
      return null;
    }

    OutputStream out = null;

    try {
      // for posting data
      conn.setDoOutput(true);

      out = new BufferedOutputStream(conn.getOutputStream());
      out.write(data.getBytes("utf-8"));
      out.flush();

      return conn;

    } catch (IOException e) {
      Log.e(TAG, "exception: " + e.toString());
    }

    if (out != null) {
      try {
        out.close();
      } catch (IOException e) {
      }
    }
    if (conn != null) {
      conn.disconnect();
    }

    return null;
  }

  private static int mLastSequence = 0;

  private static int generateSequence() {
    // --- FOR DEBUGGING ONLY ---
    if (true) {
      return 1;
    }
    // --- FOR DEBUGGING ONLY ---
    if (mLastSequence >= 0xFFFFFF) {
      mLastSequence = 0;
    }

    mLastSequence += 1;

    return mLastSequence;
  }
}