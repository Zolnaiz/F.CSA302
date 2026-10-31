/*
 * Transaction.java — Лаб 7, Programming Problem 8.33 (Figure 8.7)
 *
 * transaction(from, to, amount) хоёр дансны түгжээг авдаг. Хоёр урсгал зэрэг
 *   transaction(A, B, …) ба transaction(B, A, …)
 * хийвэл нэг нь A-г, нөгөө нь B-г барьж бие биенээ хүлээнэ → deadlock (circular wait).
 *
 * Шийдэл: түгжээг System.identityHashCode()-оор эрэмбэлж авна — бүх урсгал
 * ижил дарааллаар авах тул тойрог хүлээлт үүсэхгүй (§8.5.4). Хоёр объектын
 * hash код тэнцүү байвал (ховор) нэмэлт "tieLock"-ийг эхэлж авна.
 *
 *   javac Transaction.java
 *   java Transaction ordered     — эрэмбэлсэн: бүгд дуусна, нийт мөнгө хадгалагдана
 *   java Transaction naive       — Figure 8.7 шиг: deadlock (3 с-ийн дараа илрүүлж зогсооно)
 */
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

public class Transaction {
    static class Account {
        final String name;
        final ReentrantLock lock = new ReentrantLock();
        long balance;
        Account(String name, long balance) { this.name = name; this.balance = balance; }
        void withdraw(long amount) { balance -= amount; }
        void deposit(long amount)  { balance += amount; }
    }

    private static final Object tieLock = new Object();

    /** Figure 8.7: эхлээд from, дараа нь to — дуудагчаас хамаарч дараалал өөр. */
    static void naiveTransaction(Account from, Account to, long amount) {
        from.lock.lock();
        try {
            pause();                                  // deadlock-ийн цонхыг томруулна
            to.lock.lock();
            try {
                from.withdraw(amount);
                to.deposit(amount);
            } finally { to.lock.unlock(); }
        } finally { from.lock.unlock(); }
    }

    /** 8.33: identityHashCode-оор эрэмбэлж түгжинэ — бүх урсгал ижил дараалал. */
    static void orderedTransaction(Account from, Account to, long amount) {
        int fh = System.identityHashCode(from), th = System.identityHashCode(to);
        if (fh < th) {
            lockBoth(from, to, from, to, amount);
        } else if (fh > th) {
            lockBoth(to, from, from, to, amount);
        } else {
            synchronized (tieLock) {                  // hash тэнцүү — нэмэлт түгжээгээр дараалал тогтооно
                lockBoth(from, to, from, to, amount);
            }
        }
    }

    private static void lockBoth(Account first, Account second, Account from, Account to, long amount) {
        first.lock.lock();
        try {
            pause();
            second.lock.lock();
            try {
                from.withdraw(amount);
                to.deposit(amount);
            } finally { second.lock.unlock(); }
        } finally { first.lock.unlock(); }
    }

    private static void pause() {
        try { Thread.sleep(1); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
    }

    public static void main(String[] args) throws Exception {
        boolean ordered = args.length == 0 || !args[0].equals("naive");
        Account a = new Account("A", 1000), b = new Account("B", 1000);
        final int rounds = 200;
        System.out.printf("Горим: %s; identityHashCode A=%d, B=%d%n",
                ordered ? "ordered (8.33)" : "naive (Figure 8.7)",
                System.identityHashCode(a), System.identityHashCode(b));

        Runnable aToB = () -> { for (int i = 0; i < rounds; i++)
            if (ordered) orderedTransaction(a, b, 1); else naiveTransaction(a, b, 1); };
        Runnable bToA = () -> { for (int i = 0; i < rounds; i++)
            if (ordered) orderedTransaction(b, a, 1); else naiveTransaction(b, a, 1); };

        Thread t1 = new Thread(aToB, "A->B"), t2 = new Thread(bToA, "B->A");
        t1.setDaemon(true); t2.setDaemon(true);       // deadlock болсон ч JVM гарч чадна
        long start = System.nanoTime();
        t1.start(); t2.start();
        t1.join(TimeUnit.SECONDS.toMillis(3));
        t2.join(TimeUnit.SECONDS.toMillis(3));

        if (t1.isAlive() || t2.isAlive()) {
            System.out.println("DEADLOCK: 3 с-ийн дараа ч урсгалууд бие биенийхээ түгжээг хүлээж байна");
            System.out.printf("  A түгжээг барьсан: %b, B түгжээг барьсан: %b%n", a.lock.isLocked(), b.lock.isLocked());
            System.exit(2);
        }
        System.out.printf("Бүх %d гүйлгээ дууслаа (%.2f с). A=%d, B=%d, нийт=%d (1000+1000 хадгалагдсан: %b)%n",
                2 * rounds, (System.nanoTime() - start) / 1e9, a.balance, b.balance,
                a.balance + b.balance, a.balance + b.balance == 2000);
    }
}
