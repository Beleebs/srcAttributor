#include <iostream>

int foobar(int, int);

int main() {
    int a = 20;
    int b = 30;
    int c = foobar(a, b);
    if (c < 100) {
        std::cout << "Ding" << std::endl;
    }
}

int foobar(int x, int y) {
    return x + y;
}