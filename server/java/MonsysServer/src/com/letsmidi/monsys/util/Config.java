package com.letsmidi.monsys.util;

import java.util.Map;

public class Config {
    private final Map<String, String> key_value_map_;

    public Config(Map<String, String> key_value_map) {
        key_value_map_ = key_value_map;
    }

    public String getString(String key, String def) {
        return key_value_map_.getOrDefault(key, def);
    }

    public int getInt(String key, int def) {
        String s = key_value_map_.getOrDefault(key, null);
        if (s == null) {
            return def;
        }

        try {
            return Integer.valueOf(s);
        } catch (NumberFormatException e) {
            return def;
        }
    }
}
