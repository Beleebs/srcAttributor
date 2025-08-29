// For loops
#include <iostream>

int main() {
    int x = 10;
    int y = 20;
    for (int i = 0; i < x; i++) {
        y += 5;
    }
    std::cout << "y = " << y << std::endl;
}