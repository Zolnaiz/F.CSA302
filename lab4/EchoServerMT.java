/*
 * 4.29 — 3.25-ийн echo server-ийг клиент бүрийг тусдаа урсгалаар
 * үйлчилдэг болгосон хувилбар. Байтын түвшинд (InputStream) уншдаг тул
 * binary өгөгдлийг ч зөв буцаана. read() == -1 бол клиент холболтоо хаасан.
 *
 *   java EchoServerMT.java          (port 6007)
 *   nc 127.0.0.1 6007               (өөр терминалд, хэд хэдэн клиент зэрэг)
 */
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class EchoServerMT {
    public static void main(String[] args) {
        try (ServerSocket sock = new ServerSocket(6007)) {
            System.out.println("Echo server listening on port 6007");
            while (true) {
                Socket client = sock.accept();
                new Thread(() -> echo(client)).start();
            }
        } catch (java.io.IOException ioe) {
            System.err.println(ioe);
        }
    }

    private static void echo(Socket client) {
        String name = Thread.currentThread().getName();
        System.out.println(name + " connected: " + client.getRemoteSocketAddress());
        try (client;
             InputStream in = client.getInputStream();
             OutputStream out = client.getOutputStream()) {
            byte[] buf = new byte[1024];
            int n;
            while ((n = in.read(buf)) != -1) {   /* -1 => клиент хаасан */
                out.write(buf, 0, n);
                out.flush();
            }
        } catch (java.io.IOException ioe) {
            System.err.println(name + ": " + ioe);
        }
        System.out.println(name + " disconnected");
    }
}
