#pragma once

#include "chunk.h"
#include "token.h"
#include "compiler.h"
#include <memory>
#include <stack>

enum class InterpretResult : std::uint8_t {
    Ok,
    CompileError,
    RuntimeError,
};

// we have to push b in the error case since we can't peek the stack beforehand
#define BINARY_OP(op) \
    do { \
      auto b = stack.top(); \
      if (not std::holds_alternative<Number>(b)) { \
        fmt::print(stderr, "todo make this a runtime_error ..."); \
        return InterpretResult::RuntimeError; \
      }\
      stack.pop(); \
      auto a = stack.top(); \
      if (not std::holds_alternative<Number>(a)) {\
        fmt::print(stderr, "todo make this a runtime_error aswell ...");\
        stack.push(b); \
        return InterpretResult::RuntimeError; \
      }\
      stack.pop(); \
      const auto a_value = std::get<Number>(a);\
      const auto b_value = std::get<Number>(b);\
      stack.push(a_value op b_value); \
    } while (false)

class VM {
public:
    VM() = default;
    [[nodiscard]] InterpretResult interpret(const std::string_view source);
private:
    [[nodiscard]] InterpretResult run();
private:
    std::unique_ptr<Chunk> m_chunk;
    std::vector<std::uint8_t>::const_iterator m_ip;
    std::stack<Value> stack;
};
