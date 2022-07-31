#include "compiler.h"

void Compiler::compile(const std::string_view source) {
    scanner = Scanner(source);
    std::size_t line = 1000;
    while (true) {
        Token token = scanner.scanToken();
        if (token.line != line) {
            fmt::print("{:4d}", token.line);
            line = token.line;
        } else {
            fmt::print("   | ");
        }

        fmt::print("{} '{:.{}}'\n", token.type, token.start, token.length);

        if (token.type == TokenType::Eof) {
            break;
        }
    }
}