use std::io;// 调用std库，预导入(use 显式导入)

fn main() {
    println!("Guess Number!");// 宏print
    let mut guess_number = String::new();// 声明一个可变的字符串变量

    io::stdin().read_line(&mut guess_number)
    .expect("Couldn't read line");
    // 调用io库中的stdin()方法，内置的read_line()去读取内容，引用可变的读取到guessNumber内
    // 在方法内处理错误

    println!("The number of your guessed is : {}", guess_number);// 格式化输出
}
