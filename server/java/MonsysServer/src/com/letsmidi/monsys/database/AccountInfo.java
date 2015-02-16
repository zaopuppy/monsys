package com.letsmidi.monsys.database;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;

/**
 * Created by zero on 8/21/14.
 */
@Entity
@Table(name = "account_info")
public class AccountInfo {
    public enum Status {
        NORMAL(0),
        INVALID(1);

        private final int mValue;

        Status(int value) {
            mValue = value;
        }

        public int value() {
            return mValue;
        }
    }

    @Id
    @Column(name = "account")
    private String account = null;

    @Column(name = "password", nullable = false)
    private String password = null;

    @Column(name = "salt", nullable = false)
    private String salt = null;

    @Column(name = "nickname", nullable = false)
    private String nickname = null;

    @Column(name = "fgw_list", nullable = false)
    private String fgwList = null;

    @Column(name = "status", nullable = false)
    private int status = 0;

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

    public String getFgwList() {
        return fgwList;
    }

    public void setFgwList(String fgwList) {
        this.fgwList = fgwList;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }
}
