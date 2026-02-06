#include "functions.hpp"
#include <iostream>

int main() {
    int num = 20;
    foo(num);
    std::cout << "After pass by reference" << num << std::endl;
    std::cout << "Pass by value: " << bar(num) << std::endl;
    std::cout << "After pass by value: " << num << std::endl;
}