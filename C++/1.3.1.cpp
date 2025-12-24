#include <iostream>
#include <iomanip> // 包含iomanip头文件以使用setw等操作符

int main() {
    std::cout << "Hello";
    std::cout.width(10); // 设置宽度为10
    std::cout << "World" << std::endl; // 输出 "     World" (5个空格 + World)

    std::cout << std::setw(5) << 123 << std::endl; // 输出 "  123" (2个空格 + 123)
    return 0;
}