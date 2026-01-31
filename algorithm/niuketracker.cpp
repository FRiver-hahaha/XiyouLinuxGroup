#include <iostream>
using namespace std;

int main() {
    char ar[7];
    cin >> ar;
    if(ar[5] >= '3' && ar[5] <= '5') {
        cout << "spring" << endl;
    }else if(ar[5] >= '6' && ar[5] <= '8') {
        cout << "summer" << endl;
    }else if(ar[5] >= '9' && ar[5] <= '11') {
        cout << "autumn" << endl;
    }else if(ar[5] == '12' || ar[5] == '1' || ar[5] == '2') {
        cout << "winter" << endl;
    }

    return 0;
}