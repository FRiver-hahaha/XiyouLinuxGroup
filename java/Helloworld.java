class Dog {
    private int age;
    private String name = "dog";
    
    void woof() {
        System.out.println("nihao");
    }

    void age() {
        age = 1;
        System.out.println(age + name);
    }

    void bite(String animal) {
        System.out.println(animal);
    }
}

 class MyDog extends Dog {

}

public class Helloworld {
    public static void main(String[] args) {
        System.out.println("helloworld");
        Dog d1 = new Dog();
        d1.woof();
        d1.age();
        d1.bite("cat");

        MyDog d2 = new MyDog();
        d2.woof();
        d2.age();
        d2.bite("cat");
    }

    
}