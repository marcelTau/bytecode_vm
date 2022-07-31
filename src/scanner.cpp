#include "scanner.h"
#include <cassert>

Scanner::Scanner(const std::string_view source) {
    start = source.begin();
    current = source.begin();
    line = 1;
}

Token Scanner::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::Eof);
    }

    char c = advance();

    switch (c) {
        case '(': return makeToken(TokenType::LeftParen);
        case ')': return makeToken(TokenType::RightParen);
        case '{': return makeToken(TokenType::LeftBrace);
        case '}': return makeToken(TokenType::RightBrace);
        case ';': return makeToken(TokenType::Semicolon);
        case ',': return makeToken(TokenType::Comma);
        case '.': return makeToken(TokenType::Dot);
        case '-': return makeToken(TokenType::Minus);
        case '+': return makeToken(TokenType::Plus);
        case '/': return makeToken(TokenType::Slash);
        case '*': return makeToken(TokenType::Star);
        case '!': return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
        case '=': return makeToken(match('=') ? TokenType::EqualEquaL : TokenType::Equal);
        case '<': return makeToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        case '>': return makeToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
    }


    return errorToken("Unexpected character.");
}

bool Scanner::isAtEnd() const {
    return *current == '\0';
}

char Scanner::peek() const {
    return *current;
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            if (c == '\n') {
                line++;
            }
            std::ignore = advance();
        } else {
            return;
        }
    }
}

char Scanner::advance() {
    current++;
    return *(current - 1);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }

    if (*current != expected) {
        return false;
    }

    current++;
    return true;
}

Token Scanner::errorToken(const char *msg) const {
    return Token {
        .type { TokenType::Error },
        .start { msg },
        .length { std::strlen(msg) },
        .line { line },
    };
}

Token Scanner::makeToken(const TokenType& type) const {
    long distance = std::distance(start, current);
    assert(distance >= 0);
    return Token {
        .type { type },
        .start { start },
        .length { static_cast<std::size_t>(distance) },
        .line { line },
    };
}
