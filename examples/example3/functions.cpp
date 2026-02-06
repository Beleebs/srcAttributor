#include "functions.hpp"

void foo(int& x) {
    x += 100;
}

int bar(int y) {
    y += 100;
    return y;
}