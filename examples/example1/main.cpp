#include "functions.hpp"
#include <iostream>

int main() {
    int input1;
    char input2;

    std::cout << "please input a number: ";
    std::cin >> input1;
    std::cout << "please input a character: ";
    std::cin >> input2;

    function_one(input1, input2);
}