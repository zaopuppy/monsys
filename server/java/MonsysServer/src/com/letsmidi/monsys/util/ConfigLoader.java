package com.letsmidi.monsys.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class ConfigLoader {
    public static Config loadAsConfig(File file) {
        try {
            return new Config(loadAsMap(file));
        } catch (MonsysException e) {
            return null;
        }
    }

    public static Map<String, String> loadAsMap(File file) throws MonsysException {
        Map<String, String> map = new HashMap<>();

        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(file));
            for (String line = reader.readLine(); line != null; line = reader.readLine()) {
                line = line.trim();
                if (line.length() <= 0 || line.startsWith("#")) {
                    continue;
                }

                int idx = line.indexOf('=');
                if (idx <= 0) {
                    throw new MonsysException("bad line: [" + line + "]");
                }

                String k = line.substring(0, idx).trim();
                String v = line.substring(idx+1, line.length()).trim();
                System.out.println(String.format("key=%s, value=%s", k, v));

                if (map.containsKey(k)) {
                    throw new MonsysException("key alread exists: " + k);
                }

                map.put(k, v);
            }

            return map;
        } catch (IOException e) {
            e.printStackTrace();
            throw new MonsysException(e.toString());
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

}
