// pass by reference VS pass by value
#include <iostream>

int pbv(int);
void pbr(int&);

int main() {
    int x = 10;
    int y = pbv(x);
    pbr(x);
    std::cout << "y = " << y << std::endl;
    std::cout << "x = " << x << std::endl;
}

int pbv(int v) {
    int a = v * 2;
    return a;
}

void pbr(int& r) {
    r = 40;
}