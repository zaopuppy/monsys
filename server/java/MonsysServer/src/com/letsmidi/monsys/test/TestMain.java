package com.letsmidi.monsys.test;


import java.io.File;
import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Logger;

import com.letsmidi.monsys.log.LogFormatter;
import com.letsmidi.monsys.util.BeanLoader;
import com.letsmidi.monsys.util.MonsysException;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.boot.registry.StandardServiceRegistryBuilder;
import org.hibernate.cfg.Configuration;

/**
 * For API testing
 * <p>
 * Created by zero on 8/12/14.
 */
public class TestMain {
    public static void main(String[] args) throws IOException, MonsysException {
        TestMain t = new TestMain();

        System.out.println(String.format("%08d", 1));

        //t.testHibernate();
        //t.testMail();
        // t.testSdk();
    }

    private void testSdk() {
        //MonsysService service = MonsysService.newService();
        //service.connect(new Runnable() {
        //  @Override
        //  public void run() {
        //    // TODO: onMessage()
        //  }
        //});
        //service.login(new Runnable() {
        //  @Override
        //  public void run() {
        //  }
        //});
    }

    private void testMail() {
        //
    }

    private void testHibernate() {
        //File file = new File("src/hibernate/hibernate.cfg.xml");
        Configuration conf = new Configuration().configure("hibernate/hibernate.cfg.xml");

        StandardServiceRegistryBuilder builder =
                new StandardServiceRegistryBuilder();

        builder.applySettings(conf.getProperties());

        SessionFactory session_factory = conf.buildSessionFactory(builder.build());

        Session session = session_factory.openSession();

        try {

            Test t = new Test();
            t.setId1(1);
            t.setId1(11);
            t.setContent("the first record");

            session.beginTransaction();
            session.save(t);
            session.getTransaction().commit();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            session.close();
            session_factory.close();
        }

        System.out.println("Done");
    }

    public static class TestBean {
        private String mAbc;
        private int mIbc;

        public void setAbc(String abc) {
            mAbc = abc;
        }

        public String getAbc() {
            return mAbc;
        }

        public void setIbc(int ibc) {
            mIbc = ibc;
        }

        public int getIbc() {
            return mIbc;
        }
    }

    private void testBeanLoader() throws MonsysException {
        TestBean test = BeanLoader.load(new File("test.cfg"), TestBean.class);
        System.out.println("abc: " + test.getAbc());
        System.out.println("ibc: " + test.getIbc());
    }

    private void testLogger() throws IOException {
        Logger logger = Logger.getLogger("");

        FileHandler log_handler = new FileHandler("test.log");
        log_handler.setFormatter(new LogFormatter());

        logger.addHandler(log_handler);
        logger.severe("very good");

        Logger child_logger = Logger.getLogger(TestMain.class.getCanonicalName());
        child_logger.severe("Child's good");
    }
}
