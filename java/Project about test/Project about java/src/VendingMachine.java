public class VendingMachine {
    int price;// 价格
    int balance;// 当前余额
    int total;// 所有收入的钱
 
    void showPrompt() {
        System.out.println("Welcome");
    }

    void insertMoney(int amout) {
        balance = balance + amout;
    } 

    void showBalance() {
        System.out.println(balance);
    }

    void getFood() {
        if(balance >= price) {
            System.out.println("Here you are");
            balance -= price;
            total += price;
        }


    }
    public static void main(String[] args) throws Exception {
        System.out.println("Hello, World!");
    }
}
