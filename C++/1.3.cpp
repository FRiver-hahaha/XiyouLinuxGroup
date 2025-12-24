#include <iostream>

using namespace std;

int main() {
    int sum = 0;
    
    cout << "请输入要求和的数字：";
    
    int i;

    while(cin >> i) {
        sum += i;
        while(cin.peek() == ' ') {
            cin.get();
        }
        if(cin.peek() == '\n') break;
    }

    cout << "sum = " << sum << endl;

    return 0;
}