#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <string_view>
#include <vector>

// OpCode.h
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



// @todo refactor this
using Value = double;

struct Chunk {
    void push(OpCode opcode, std::size_t line) {
        code.push_back(static_cast<std::uint8_t>(opcode));
        lines.push_back(line);
    }

    void push(std::uint8_t opcode, std::size_t line) {
        code.push_back(opcode);
        lines.push_back(line);
    }

    void disassembleChunk(const std::string_view name) const {
        fmt::print("== {} ({}) ==\n", name, code.size());

        for (std::size_t offset = 0; offset < code.size();) {
            offset = disassembleInstruction(offset);
        }
    }

    [[nodiscard]] std::size_t disassembleInstruction(std::size_t offset) const {
        fmt::print("{:04d} ", offset);

        if (offset > 0 && lines[offset] == lines[offset - 1]) {
            fmt::print("   | ");
        } else {
            fmt::print("{:4d} ", lines[offset]);
        }

        const auto instruction = static_cast<OpCode>(code[offset]);

        switch (instruction) {
            case OpCode::Constant: return constantInstruction("Constant", offset);
            case OpCode::Add: return simpleInstruction("Add", offset);
            case OpCode::Subtract: return simpleInstruction("Subtract", offset);
            case OpCode::Multiply: return simpleInstruction("Multiply", offset);
            case OpCode::Divide: return simpleInstruction("Divide", offset);
            case OpCode::Negate: return simpleInstruction("Negate", offset);
            case OpCode::Return: return simpleInstruction("Return", offset);
            default:
                fmt::print("Unknown opcode {}\n", instruction);
                return offset + 1;
        }
    }

    [[nodiscard]] std::size_t simpleInstruction(const std::string_view name, std::size_t offset) const {
        fmt::print("{}\n", name);
        return offset + 1;
    }

    [[nodiscard]] std::size_t addConstant(const Value& value) {
        constants.push_back(value);
        return constants.size() - 1;
    }

    [[nodiscard]] std::size_t constantInstruction(const std::string_view name, std::size_t offset) const {
        std::uint8_t constant = code[offset + 1];
        fmt::print("{:16} {:4d} '{}'\n", name, constant, constants[constant]);
        return offset + 2;
    }

    std::vector<std::uint8_t> code;
    std::vector<Value> constants;
    std::vector<std::size_t> lines;
};

