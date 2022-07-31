#pragma once

#include <fmt/format.h>

enum class TokenType : std::uint8_t {
    Eof,
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
  const char* start;
  int length;
  int line;
};
