/*
 * Зураг 3.28-ийн date client — 4.26-ийн серверийг турших.
 *
 *   java DateClient.java
 */
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;

public class DateClient {
    public static void main(String[] args) {
        try (Socket sock = new Socket("127.0.0.1", 6013);
             BufferedReader bin = new BufferedReader(
                     new InputStreamReader(sock.getInputStream()))) {
            String line;
            while ((line = bin.readLine()) != null)
                System.out.println(line);
        } catch (java.io.IOException ioe) {
            System.err.println(ioe);
        }
    }
}
