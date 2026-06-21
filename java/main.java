import java.util.Scanner;

public class main {
    public static void main(String[] args) {
        Scanner Input = new Scanner(System.in);
        // String s = new String("Hello");
        // System.out.println(s + (12 + 24));// 自动把数字类型变为字符串类型

        String s = Input.next();
        System.out.println(s);
        System.out.println(s == "bye");// java中的==号比较是比较的所有权，无法比较内容 错误做法
        System.out.println(s.equals("bye"));// 比较了内容 正确做法

        String s1 = "Hello";
        String s2 = "Hello";
        System.out.println(s1 == s2);
        s1 = s2;
        System.out.println(s1 == s2);
        
    }
}