package com.letsmidi.monsys.util;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;

/**
 * Initialize a Bean instance by loading a config file
 * <p>
 * Created by zero on 8/16/14.
 */
public class BeanLoader {
    public static <T> T load(File file, Class<T> clazz) throws MonsysException {
        // read all configuration from file
        Map<String, String> kv_map = ConfigLoader.loadAsMap(file);

        // use configuration to initialize instance
        try {
            T obj = clazz.newInstance();

            Method[] methods = clazz.getMethods();

            for (Map.Entry<String, String> entry : kv_map.entrySet()) {
                String method_name = toMethodNameSet(entry.getKey());
                Method method = getMethod(methods, method_name);
                if (method == null) {
                    throw new MonsysException("No such configure item: " + entry.getKey());
                }
                Class<?>[] types = method.getParameterTypes();
                if (types.length != 1) {
                    throw new MonsysException("Bad set method, param count: " + types.length);
                }
                if (types[0].getName().equals("java.lang.String")) {
                    method.invoke(obj, entry.getValue());
                } else if (types[0].getName().equals("int")) {
                    method.invoke(obj, Integer.parseInt(entry.getValue()));
                } else {
                    throw new MonsysException("type[" + types[0].getName() + "] is not supported yet");
                }
            }

            return obj;
        } catch (InstantiationException | IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
        }

        return null;
    }

    private static Method getMethod(Method[] methods, String method_name) {
        for (Method m : methods) {
            if (m.getName().equals(method_name)) {
                return m;
            }
        }

        return null;
    }

    private static String toMethodNameSet(String key) {
        return "set" + key;
    }
}

