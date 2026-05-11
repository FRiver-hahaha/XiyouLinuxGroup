// import java.util.Scanner;

public class test {
    public static void main(String[] args) {
        boolean[] primes = new boolean[100];
        for(int i = 0; i < primes.length; ++i) {
            primes[i] = true;
        }
        primes[0] = primes[1] = false;

        for(int i = 2; i < primes.length; ++i) {
            if(primes[i]) {
                for(int k = 2; i * k < primes.length; ++k) {
                    primes[i * k] = false;
                }
            }
        }

        for(int i = 0; i < primes.length; ++i) {
            if(primes[i]) {
                System.out.println(i);
            }
        }
    }
}