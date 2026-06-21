public class Test {
     public static void main(String args[]){
          String str="ABCDE";
          str.substring(3);
          str.concat("XYZ");
          System.out.print(str);
          StringBuffer buf=new StringBuffer("abcde");
          buf.reverse();
          System.out.println(buf);
     }
}