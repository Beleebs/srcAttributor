#include "functions.hpp"
#include <iostream>

void function_one(int a, char b) {
    if (a > 5) {
        std::cout << "Less than 5. " << b << std::endl;
    }
}