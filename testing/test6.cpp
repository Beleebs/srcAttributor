#include <iostream>

// Void function to print numbers from 1 to n
void printNumbers(int n) {
    for(int i = 1; i <= n; i++) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

// Function that returns the sum of numbers from 1 to n
int sumNumbers(int n) {
    int sum = 0;
    for(int i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main() {
    int num = 5;

    std::cout << "Printing numbers up to " << num << ": ";
    printNumbers(num);

    std::cout << "Sum of numbers up to " << num << ": " << sumNumbers(num) << std::endl;
}