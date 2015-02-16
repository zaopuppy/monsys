package com.letsmidi.monsys.database;

import java.util.Date;

/**
 * Created by zero on 8/21/14.
 */
public class TempAccountInfo {
    private String account = null;
    private String password = null;
    private String salt = null;
    private String nickname = null;
    private Date expire = null;

    public String getAccount() {
        return account;
    }

    public void setAccount(String account) {
        this.account = account;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getSalt() {
        return salt;
    }

    public void setSalt(String salt) {
        this.salt = salt;
    }

    public String getNickname() {
        return nickname;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public Date getExpire() {
        return expire;
    }

    public void setExpire(Date expire) {
        this.expire = expire;
    }
}
