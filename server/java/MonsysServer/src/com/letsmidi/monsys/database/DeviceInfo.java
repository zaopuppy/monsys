package com.letsmidi.monsys.database;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;

/**
 * Created by zero on 6/28/15.
 */
@Entity
@Table(name="device_info")
public class DeviceInfo {

    @Id
    @Column(name="device_id")
    private String deviceId = null;

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }
}
