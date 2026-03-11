#include <iostream>
using namespace std;

int main() {
    char c;
    cin >> c;
    int arr[10];
    int point = 0;
    int cnt = 0;
    while(c > 0) {
        if(c & 1) {
            arr[cnt++] = point;
        }
        point++;
        c >>= 1;
    }
    for(int i = 7; i >= 0; --i) {
        int is1 = 0;
        for(int j = 0; j < cnt; j++) {
            if(arr[j] == i) {
                is1 = 1;
                break;
            }
        }
        if(is1) {
            cout << '1';
        }else {
            cout << '0';
        }
    }
    cout << endl;

    return 0;
}
