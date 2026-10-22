/*
 * 4.26 — Зураг 3.27-ийн date server-ийг клиент бүрийг тусдаа урсгалаар
 * үйлчилдэг болгосон хувилбар.
 *
 *   java DateServerMT.java          (port 6013)
 *   java DateClient.java            (өөр терминалд)
 */
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class DateServerMT {
    public static void main(String[] args) {
        try (ServerSocket sock = new ServerSocket(6013)) {
            System.out.println("Date server listening on port 6013");
            while (true) {
                Socket client = sock.accept();
                /* клиент бүрт шинэ урсгал */
                new Thread(() -> serve(client)).start();
            }
        } catch (java.io.IOException ioe) {
            System.err.println(ioe);
        }
    }

    private static void serve(Socket client) {
        try (client; PrintWriter pout = new PrintWriter(client.getOutputStream(), true)) {
            System.out.println(Thread.currentThread().getName()
                    + " serving " + client.getRemoteSocketAddress());
            pout.println(new java.util.Date().toString());
        } catch (java.io.IOException ioe) {
            System.err.println(ioe);
        }
    }
}
