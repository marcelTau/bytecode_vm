#include <gtest/gtest.h>

#include "chunk.h"
#include "compiler.h"
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

TEST(compiler, check_the_concept) {
    Chunk c;
    Compiler cc(c);

    std::ignore = cc.compile("print 1 + 2;");
    EXPECT_EQ(true, false);
}









