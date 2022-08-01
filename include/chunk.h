#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <string_view>
#include <vector>
#include <variant>
#include "opcode.h"

using Number = double;
using Nil = std::monostate;
using Value = std::variant<bool, Number, Nil>;

struct PrintVisitor {
    std::string operator()(bool t) { return t ? "true" : "false"; }
    std::string operator()(Number t) { return std::to_string(t); }
    std::string operator()(Nil) { return "Nil"; }
};

struct Chunk {
    void push(OpCode opcode, std::size_t line);
    void push(std::uint8_t opcode, std::size_t line);
    void disassembleChunk(const std::string_view name) const;

    [[nodiscard]] std::size_t disassembleInstruction(std::size_t offset) const;
    [[nodiscard]] std::size_t simpleInstruction(const std::string_view name, std::size_t offset) const;

    [[nodiscard]] std::size_t addConstant(const Value& value);
    [[nodiscard]] std::size_t constantInstruction(const std::string_view name, std::size_t offset) const;

    std::vector<std::uint8_t> code;
    std::vector<Value> constants;
    std::vector<std::size_t> lines;
};

