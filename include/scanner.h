#pragma once

#include "token.h"
#include <string_view>

struct Scanner {
    Scanner() = default;
    Scanner(const std::string_view source);

    [[nodiscard]] Token scanToken();

private:
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] Token makeToken(const TokenType& type) const;
    [[nodiscard]] Token errorToken(const char *msg) const;
    [[nodiscard]] char advance();
    [[nodiscard]] bool match(char expected);

public:
    std::string_view::iterator start;
    std::string_view::iterator current;
    std::size_t line;
};
