#include <fmt/format.h>
#include <iostream>
#include "header.h"

int main() {
    fmt::print("Hello, World {} !\n", add_2(10));
    branches(10);
    return 0;
}
