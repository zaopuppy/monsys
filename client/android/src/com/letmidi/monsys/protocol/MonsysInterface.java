package com.letmidi.monsys.protocol;

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

import com.letmidi.monsys.DevInfo;
import com.letmidi.monsys.FgwInfo;

public class MonsysInterface {
  private static final String TAG = "XXX";
  private static final String SERVER_LOGIN_HOST = "http://letsmidi.wicp.net:1983/interface/login";
  private static final String SERVER_HOST = "http://letsmidi.wicp.net:1983/interface";
  private static String mCookies = null;

  public static void logout() {
    mCookies = null;
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
      if (conn != null) { conn.disconnect(); }
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

    if (out != null) { try { out.close(); } catch (IOException e) {} }
    if (conn != null) { conn.disconnect(); }

      return null;
  }


  public static boolean login(String account, String password) {
    HttpURLConnection conn = post(SERVER_LOGIN_HOST, "account=ztest%40gmail.com&password=123");
    if (conn == null) {
      return false;
    }

    try {
      int status = conn.getResponseCode();
      String rsp = conn.getResponseMessage();

      Log.d(TAG, "received(" + status + "): " + rsp);

      saveCookies(conn.getHeaderFields());

      return true;

    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (conn != null) { conn.disconnect(); }
    }

    return false;
  }

  public static List<FgwInfo> getFgwList(String account) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-fgw-list");
      jreq.put("account", account);
      jreq.put("uid", "456");
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return null;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return null;
    }

    InputStream in = null;
    try {
      int status = conn.getResponseCode();
      String rsp = conn.getResponseMessage();

      Log.d(TAG, "response: (" + status + "): " + rsp);

      in = new BufferedInputStream(conn.getInputStream());

      byte[] buffer = new byte[1024];
      int rv = in.read(buffer);

      String message = new String(buffer, 0, rv, "utf-8");

      Log.d(TAG, "received: [" + message + "]");

      JSONObject jrsp = new JSONObject(message);
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
    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (in != null) { try { in.close(); } catch (IOException e) {} }
      if (conn != null) { conn.disconnect(); }
    }

    return null;
  }

  public static List<DevInfo> getDevList(String fgw_id) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-dev-list");
      jreq.put("fgw", fgw_id);
      jreq.put("uid", "456");
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
      return null;
    }

    HttpURLConnection conn = post(SERVER_HOST, jreq.toString());
    if (conn == null) {
      return null;
    }

    InputStream in = null;
    try {
      int status = conn.getResponseCode();
      String rsp = conn.getResponseMessage();

      Log.d(TAG, "response: (" + status + "): " + rsp);

      in = new BufferedInputStream(conn.getInputStream());

      byte[] buffer = new byte[1024];
      int rv = in.read(buffer);

      String message = new String(buffer, 0, rv, "utf-8");

      Log.d(TAG, "received: [" + message + "]");

      JSONObject jrsp = new JSONObject(message);
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
    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (in != null) { try { in.close(); } catch (IOException e) {} }
      if (conn != null) { conn.disconnect(); }
    }

    return null;
  }

  public static List<Pair<Integer, Integer>> getDevInfo(String fgw_id, int dev_addr, List<Integer> ids) {
    JSONObject jreq = new JSONObject();
    try {
      jreq.put("cmd", "get-dev-info");
      jreq.put("fgw", fgw_id);
      jreq.put("addr", dev_addr);
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

    InputStream in = null;
    try {
      int status = conn.getResponseCode();
      String rsp = conn.getResponseMessage();

      Log.d(TAG, "response: (" + status + "): " + rsp);

      in = new BufferedInputStream(conn.getInputStream());

      byte[] buffer = new byte[1024];
      int rv = in.read(buffer);

      String message = new String(buffer, 0, rv, "utf-8");

      Log.d(TAG, "received: [" + message + "]");

      JSONObject jrsp = new JSONObject(message);
      JSONArray jid_infos = jrsp.getJSONArray("id-infos");

      List<Pair<Integer, Integer>> info_list = new LinkedList<Pair<Integer, Integer>>();
      for (int i = 0; i < jid_infos.length(); ++i) {
        JSONObject jobj = jid_infos.getJSONObject(i);

        info_list.add(
            new Pair<Integer, Integer>(
                jobj.getInt("id"), jobj.getInt("value")));
      }

      return info_list;
    } catch (IOException e) {
      Log.e(TAG, e.toString());
    } catch (JSONException e) {
      Log.e(TAG, e.toString());
    } finally {
      if (in != null) { try { in.close(); } catch (IOException e) {} }
      if (conn != null) { conn.disconnect(); }
    }

    return null;
  }

  private static void saveCookies(Map<String, List<String>> headers) {
    // extract all cookies and fiter duplicated ones
    Map<String, String> key_map = new HashMap<String, String>(10);
    String[] fields;
    String[] kv;
    for (Map.Entry<String, List<String>> entry: headers.entrySet()) {
      if ("Set-Cookie".equals(entry.getKey())) {
        // "account=enRlc3RAZ21haWwuY29t|1388901264|0930497a9d4b93d45026601af43a0191edf85bab; expires=Tue, 04 Feb 2014 05:54:24 GMT; Path=/"
        for (String value: entry.getValue()) {
          fields = value.split(";");
          for (String field: fields) {
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

    // join them~
    StringBuilder builder = new StringBuilder(512);
    boolean first = true;
    for (Map.Entry<String, String> entry: key_map.entrySet()) {
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

    Log.d(TAG, "final cookies is: [" + mCookies + "]");
  }
}
