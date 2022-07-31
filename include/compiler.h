#pragma once

#include <string_view>
#include "scanner.h"

struct Compiler {
    Compiler() = default;
    void compile(const std::string_view source);

private:
    Scanner scanner;
};
