#pragma once

#include "token.h"
#include <string_view>

struct Scanner {
    Scanner() = default;
    Scanner(const std::string_view source);

    [[nodiscard]] Token scanToken();

private:
    [[nodiscard]] Token makeToken(const TokenType& type) const;
    [[nodiscard]] Token errorToken(const char *msg) const;

    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] char advance();
    [[nodiscard]] char peek() const;
    [[nodiscard]] char peekNext() const;
    [[nodiscard]] bool match(char expected);
    [[nodiscard]] bool isAlpha(char c);
    [[nodiscard]] TokenType identifierType();

    [[nodiscard]] Token string();
    [[nodiscard]] Token number();
    [[nodiscard]] Token identifier();

    void skipWhitespace();

public:
    std::string_view::iterator m_start;
    std::string_view::iterator m_current;
    std::size_t m_line;
};
