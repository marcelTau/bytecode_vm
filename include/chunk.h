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

/**
* @brief Visitor that returns the string representation of every case in the Value variant.
*        IMPORTANT all the types in the Value variant have to be printable by fmt::format or
*        should have a special case defined.
*/
struct PrintVisitor {
    // The Nil (std::monostate) variant cannot be formatted by fmt::format by default, so we can catch it here.
    std::string operator()(Nil) { return "Nil"; }
    std::string operator()(bool b) { return b ? "true" : "false"; }

    // This matches every other type than std::monostate. 
    std::string operator()(const auto& x) { return fmt::format("{}", x); }
};

struct EqualityVisitor {
    template<typename T>
    bool operator()(const T& a, const T& b) {
        return a == b;
    }

    template<typename T, typename U>
    bool operator()(const T&, const U&) {
#ifdef DEBUG_PRINT_CODE
        fmt::print(stderr, "Equality operator failed with different types.");
#endif
        return false;
    }
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

