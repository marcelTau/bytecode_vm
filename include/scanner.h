#pragma once

#include "token.h"
#include <string_view>

struct Scanner {
    Scanner() = default;
    Scanner(const char *source);

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
    [[nodiscard]] TokenType checkKeyword(std::size_t start, std::size_t length, const char *rest, TokenType type);

    [[nodiscard]] Token string();
    [[nodiscard]] Token number();
    [[nodiscard]] Token identifier();

    void skipWhitespace();

public:
    const char *m_start;
    const char *m_current;
    std::size_t m_line;
};
