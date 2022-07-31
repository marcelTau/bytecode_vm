#include "scanner.h"
#include <cassert>

Scanner::Scanner(const std::string_view source) {
    m_start = source.begin();
    m_current = source.begin();
    m_line = 1;
}

Token Scanner::scanToken() {
    skipWhitespace();
    m_start = m_current;

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
    return *m_current == '\0';
}

char Scanner::peek() const {
    return *m_current;
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            if (c == '\n') {
                m_line++;
            }
            std::ignore = advance();
        } else {
            return;
        }
    }
}

char Scanner::advance() {
    m_current++;
    return *(m_current - 1);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }

    if (*m_current != expected) {
        return false;
    }

    m_current++;
    return true;
}

Token Scanner::errorToken(const char *msg) const {
    return Token {
        .type { TokenType::Error },
        .start { msg },
        .length { std::strlen(msg) },
        .line { m_line },
    };
}

Token Scanner::makeToken(const TokenType& type) const {
    long distance = std::distance(m_start, m_current);
    assert(distance >= 0);
    return Token {
        .type { type },
        .start { m_start },
        .length { static_cast<std::size_t>(distance) },
        .line { m_line },
    };
}
