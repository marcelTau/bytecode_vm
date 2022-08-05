#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <string_view>
#include <vector>
#include <variant>
#include "opcode.h"

// StackAllocated DataTypes
using Number = double;
using Bool = bool;
using Nil = std::monostate;

// HeapAllocated DataTypes
using Obj = std::variant<std::string, std::monostate>;

// Value Variant holding all types
using Value = std::variant<Bool, Number, Nil, Obj>;

template <typename ObjType>
static bool holds_obj_type(const Value& value) {
    return (std::holds_alternative<Obj>(value) && std::holds_alternative<ObjType>(std::get<Obj>(value)));
}

template <typename ObjType>
static ObjType get_objtype_unchecked(const Value& value) {
    return std::get<ObjType>(std::get<Obj>(value));
}

/**
 * @brief Visitor that returns the string representation of every case in the Value variant.
 *        IMPORTANT all the types in the Value variant have to be printable by fmt::format or
 *        should have a special case defined.
 */
struct PrintVisitor {
    // Base case for Obj Value
    std::string operator()(const Obj& obj) { return std::visit(PrintVisitor{}, obj); }

    // The Nil (std::monostate) variant cannot be formatted by fmt::format by default, so we can catch it here.
    std::string operator()(Nil) { return "Nil"; }
    std::string operator()(Bool b) { return b ? "true" : "false"; }

    // This matches every other type than std::monostate. 
    std::string operator()(const auto& x) { return fmt::format("{}", x); }
};

/**
 * @brief Visitor that checks if the values holds in two different
 *        std::variants are the same type and the same value. It returns false
 *        in every other case.
 *
 */
struct EqualityVisitor {
    // Base case for Obj Value
    bool operator()(const Obj& a, const Obj& b) {
        return std::visit(EqualityVisitor{}, a, b);
    }

    // If the types are the same, they have to implement operator==
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

