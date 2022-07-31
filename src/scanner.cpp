#include "scanner.h"
#include <cassert>
#include <cctype>

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

    if (std::isdigit(c)) {
        return number();
    }

    if (isAlpha(c)) {
        return identifier();
    }

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
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || std::isdigit(peek())) {
        std::ignore = advance();
    }

    return makeToken(identifierType());
}

TokenType Scanner::identifierType() {

    switch (*m_start) {
        case 'a': return checkKeyword(1, 2, "nd", TokenType::And);
        case 'c': return checkKeyword(1, 4, "lass", TokenType::Class);
        case 'e': return checkKeyword(1, 3, "lse", TokenType::Else);
        case 'f': {
            if (std::distance(m_start, m_current) > 1) {
                switch (*(m_start + 1)) {
                    case 'a': return checkKeyword(2, 3, "lse", TokenType::False);
                    case 'o': return checkKeyword(2, 1, "r", TokenType::For);
                    case 'u': return checkKeyword(2, 1, "n", TokenType::Fun);
                }
            }
            break;
        }
        case 'i': return checkKeyword(1, 1, "f", TokenType::If);
        case 'n': return checkKeyword(1, 2, "il", TokenType::Nil);
        case 'o': return checkKeyword(1, 1, "r", TokenType::Or);
        case 'p': return checkKeyword(1, 4, "rint", TokenType::Print);
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::Return);
        case 's': return checkKeyword(1, 4, "uper", TokenType::Super);
        case 'v': return checkKeyword(1, 2, "ar", TokenType::Var);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::While);
    }

    return TokenType::Identifier;
}

TokenType Scanner::checkKeyword(std::size_t start, std::size_t length, const char *rest, TokenType type) {
    const bool sizeMatch = std::distance(m_start, m_current) == static_cast<long>(start + length);
    const bool contentMatch = std::memcmp(m_start + start, rest, length) == 0;
    if (sizeMatch && contentMatch) {
        return type;
    }
    return TokenType::Identifier;
}

Token Scanner::number() {
    while (std::isdigit(peek())) {
        std::ignore = advance();
    }

    if (peek() == '.' && std::isdigit(peekNext())) {
        std::ignore = advance();
        while (std::isdigit(peek())) {
            std::ignore = advance();
        }
    }

    return makeToken(TokenType::Number);
}

Token Scanner::string() {
    while (peek() != '"' && not isAtEnd()) {
        if (peek() == '\n') {
            m_line++;
        }
        std::ignore = advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    std::ignore = advance();
    return makeToken(TokenType::String);
}

bool Scanner::isAlpha(char c) {
    return std::isalpha(c) || c == '_';
}

bool Scanner::isAtEnd() const {
    return *m_current == '\0';
}

char Scanner::peek() const {
    return *m_current;
}

char Scanner::peekNext() const {
    if (isAtEnd()) {
        return '\0';
    }
    return *(m_current + 1);
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            if (c == '\n') {
                m_line++;
            }
            std::ignore = advance();
        } else if (c == '/' && peekNext() == '/') { // comment
            while (peek() == '/' && not isAtEnd()) {
                std::ignore = advance();
            }
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
