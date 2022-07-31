#pragma once

#include "token.h"
#include <string_view>

struct Scanner {
    Scanner() = default;
    Scanner(const std::string_view source) {
        start = source.begin();
        current = source.begin();
        line = 1;
    }

    [[nodiscard]] Token scanToken() {
        return Token();
    }

    std::string_view::iterator start;
    std::string_view::iterator current;
    std::size_t line;
};
