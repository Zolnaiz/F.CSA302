/*
 * Лаб №1 — Java хэл дээрх "Hello world".
 * javac нь bytecode (.class) үүсгэж, JVM дээр ажилладаг тул Windows, Linux хоёр дээр адилхан.
 * public класс тул файлын нэр заавал HelloWorld.java байна.
 *
 * Хоёр систем дээр:  java HelloWorld.java
 *          эсвэл:   javac HelloWorld.java && java HelloWorld
 */
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello world");
    }
}
