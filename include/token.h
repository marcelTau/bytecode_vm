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
    return fmt::format_to(ctx.out(), "TODO match on TokenType: {}", static_cast<std::uint8_t>(type));
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











