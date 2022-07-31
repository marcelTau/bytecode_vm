#pragma once

#include <fmt/format.h>

enum class TokenType : std::uint8_t {
    // Single-character tokens.
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,
    // One or two character tokens.
    Bang,
    BangEqual,
    Equal,
    EqualEquaL,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    // Literals.
    Identifier,
    String,
    Number,
    // Keywords.
    And,
    Class,
    Else,
    False,
    For,
    Fun,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,

    Error,
    Eof
};


template<>
struct fmt::formatter<TokenType> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx);

    template<typename FormatContext>
    auto format(const TokenType& type, FormatContext& ctx);
};

template<typename ParseContext>
constexpr auto fmt::formatter<TokenType>::parse(ParseContext& ctx) {
    return ctx.begin();
}

template<typename FormatContext>
auto fmt::formatter<TokenType>::format(const TokenType& type, FormatContext& ctx)
{
    using enum TokenType;
    std::string name;
    switch (type) {
        case LeftParen: name = "LeftParen"; break;
        case RightParen: name = "RightParen"; break;
        case LeftBrace: name = "LeftBrace"; break;
        case RightBrace: name = "RightBrace"; break;
        case Comma: name = "Comma"; break;
        case Dot: name = "Dot"; break;
        case Minus: name = "Minus"; break;
        case Plus: name = "Plus"; break;
        case Semicolon: name = "Semicolon"; break;
        case Slash: name = "Slash"; break;
        case Star: name = "Star"; break;
        case Bang: name = "Bang"; break;
        case BangEqual: name = "BangEqual"; break;
        case Equal: name = "Equal"; break;
        case EqualEquaL: name = "EqualEquaL"; break;
        case Greater: name = "Greater"; break;
        case GreaterEqual: name = "GreaterEqual"; break;
        case Less: name = "Less"; break;
        case LessEqual: name = "LessEqual"; break;
        case Identifier: name = "Identifier"; break;
        case String: name = "String"; break;
        case Number: name = "Number"; break;
        case And: name = "And"; break;
        case Class: name = "Class"; break;
        case Else: name = "Else"; break;
        case False: name = "False"; break;
        case For: name = "For"; break;
        case Fun: name = "Fun"; break;
        case If: name = "If"; break;
        case Nil: name = "Nil"; break;
        case Or: name = "Or"; break;
        case Print: name = "Print"; break;
        case Return: name = "Return"; break;
        case Super: name = "Super"; break;
        case This: name = "This"; break;
        case True: name = "True"; break;
        case Var: name = "Var"; break;
        case While: name = "While"; break;
        case Error: name = "Error"; break;
        case Eof: name = "Eof"; break;
    }
    return fmt::format_to(ctx.out(), "{}", name);
}

struct Token {
    TokenType type;
    std::string_view::iterator start;
    std::size_t length;
    std::size_t line;
};

[[nodiscard]] constexpr bool operator==(const Token& lhs, const Token& rhs) {
    return rhs.type == lhs.type &&
           rhs.line == lhs.line &&
           rhs.length == lhs.length &&
           rhs.start == lhs.start;
}

[[nodiscard]] [[maybe_unused]] static std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << fmt::format("TokenType: {}\nLine: {}\nLength: {}\nStart: {}", t.type, t.line, t.length, *t.start);
    return os;
}











