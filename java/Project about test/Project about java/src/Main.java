import java.util.Scanner;
import java.util.TreeSet;

public class Main {
    public static void main(String[] args) {
        Scanner Input = new Scanner(System.in);
        TreeSet<Integer> set = new TreeSet<>();

        while(Input.hasNext()) {
            String input = Input.nextLine();
            if("end".equals(input)) break;
            set.add(Integer.parseInt(input));
        }

        boolean first = true;

        for(int num : set) {
            if(!first) System.out.print(' ');
            System.out.print(num);
            first = false;
        }
        Input.close();
    }
}