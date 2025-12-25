use std::io;
use rand::Rng;

fn main() {
    println!("猜测一个数字：");

    let secret_number = rand::thread_rng().gen_range(1..=100);
 
    println!("神秘数字是：{}", secret_number);

    let mut guess = String::new();
    io::stdin().read_line(&mut guess).expect("无法读取行");

    println!("猜测的数字为：{}",guess);
}
