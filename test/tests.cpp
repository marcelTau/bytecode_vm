#include <gtest/gtest.h>

#include "chunk.h"
#include "compiler.h"
#include "scanner.h"

TEST(scanner, tokentype) {
    std::string_view lexeme = "for";
    Scanner s(lexeme.data());
    auto token = s.scanToken();
    EXPECT_EQ(token.type, TokenType::For);
}

TEST(scanner, full_compare) {
    std::string_view lexeme = "for";
    Scanner s(lexeme.data());
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

TEST(compiler, eof_token_at_the_end) {
    std::string_view sv = "1 + 2;";
    Scanner s(sv.data());

    std::vector<Token> tokens;
    while (true) {
        tokens.push_back(s.scanToken());
        if (tokens.back().type == TokenType::Eof) {
            break;
        }
    }
    EXPECT_EQ(tokens.back().type, TokenType::Eof);
}








