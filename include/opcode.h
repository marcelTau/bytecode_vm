#pragma once

#include <fmt/format.h>

enum class OpCode : std::uint8_t {
    Constant,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Return,
};

template<>
struct fmt::formatter<OpCode> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx);

    template<typename FormatContext>
    auto format(const OpCode& opcode, FormatContext& ctx);
};

template<typename ParseContext>
constexpr auto fmt::formatter<OpCode>::parse(ParseContext& ctx) {
    return ctx.begin();
}

template<typename FormatContext>
auto fmt::formatter<OpCode>::format(const OpCode& opcode, FormatContext& ctx)
{
    return fmt::format_to(ctx.out(), "{}", static_cast<std::uint8_t>(opcode));
}

