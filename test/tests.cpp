#include <gtest/gtest.h>

#include "scanner.h"

TEST(scanner, tokentype) {
    std::string_view lexeme = "for";
    Scanner s(lexeme);
    auto token = s.scanToken();
    EXPECT_EQ(token.type, TokenType::For);
}

TEST(scanner, full_compare) {
    std::string_view lexeme = "for";
    Scanner s(lexeme);
    Token expected = Token {
        .type { TokenType::For },
        .start { lexeme.begin() },
        .length { 3 },
        .line { 1 },
    };
    auto token = s.scanToken();
    EXPECT_EQ(token.type, TokenType::For);
    EXPECT_EQ(token, expected);
}
