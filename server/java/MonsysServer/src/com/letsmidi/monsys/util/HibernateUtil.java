package com.letsmidi.monsys.util;

import org.hibernate.SessionFactory;
import org.hibernate.boot.registry.StandardServiceRegistryBuilder;
import org.hibernate.cfg.Configuration;

/**
 * Created by zero on 8/21/14.
 */
public class HibernateUtil {
    private static SessionFactory mSessionFactory = null;

    public static boolean init(Class[] mapping_classes) {
        Configuration conf = new Configuration().configure("hibernate/hibernate.cfg.xml");

        for (Class clazz : mapping_classes) {
            conf.addAnnotatedClass(clazz);
        }

        StandardServiceRegistryBuilder builder =
                new StandardServiceRegistryBuilder();

        builder.applySettings(conf.getProperties());

        mSessionFactory = conf.buildSessionFactory(builder.build());


        //Session session = session_factory.openSession();

        return true;
    }

    public static SessionFactory getSessionFactory() {
        return mSessionFactory;
    }

}

