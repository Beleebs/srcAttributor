// While loops, its mainly the conditionals that are messing up
#include <iostream>

int main() {
    int x = 10;
    int y = 20;
    while (y > x) {
        ++x;
    }
    std::cout << "x = " << x << std::endl;
}
