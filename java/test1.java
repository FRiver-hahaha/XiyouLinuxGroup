import java.util.Scanner;

public class test1 {
    public static void main(String[] args) {
        Scanner Input = new Scanner(System.in);
        String s1 = "0123A56789你好";
        System.out.println(s1.substring(1));// 123456789你好
        System.out.println(s1.substring(2,9));// 2345678
        // substring中第二个参数是指到哪里停止，不计入这个位置的内容

        System.out.println(s1.indexOf("A56"));
        System.out.println(s1.indexOf('你'));
    }
}
