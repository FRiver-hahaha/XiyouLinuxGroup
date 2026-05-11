#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>

class TEST {
public:
    int square(int x) {
        return x * x;
    }

    int qube(int x) {
        return x * x * x;
    }

    int noName(int x) {
        return x - (x * 3 + 1) * 2;
    }
};

int main() {
    TEST test;
    std::stringstream ss;
    std::vector<int> input{1, 2, 3, 4, 5};
    std::vector<int> output;

    std::transform(
        input.begin(),
        input.end(),
        std::back_inserter(output),
        [&test](int x) {return test.square(x);}
    );

    for(int val : output) {
        ss << val << ' ';
    }

    std::cout << ss.str() << std::endl;

    return 0;
}