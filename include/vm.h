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
      auto b = m_stack.top(); \
      if (not std::holds_alternative<Number>(b)) { \
        runtimeError("Operands must be numbers."); \
        return InterpretResult::RuntimeError; \
      }\
      m_stack.pop(); \
      auto a = m_stack.top(); \
      if (not std::holds_alternative<Number>(a)) {\
        runtimeError("Operands must be numbers."); \
        m_stack.push(b); \
        return InterpretResult::RuntimeError; \
      }\
      m_stack.pop(); \
      const auto a_value = std::get<Number>(a);\
      const auto b_value = std::get<Number>(b);\
      m_stack.push(a_value op b_value); \
    } while (false)

class VM {
public:
    VM() = default;
    [[nodiscard]] InterpretResult interpret(const std::string_view source);
private:
    void runtimeError(const char *msg);
    [[nodiscard]] InterpretResult run();
    void resetStack(); 
    [[nodiscard]] bool isFalsey(const Value& value);

private:
    std::unique_ptr<Chunk> m_chunk;
    std::vector<std::uint8_t>::const_iterator m_ip;
    std::stack<Value> m_stack;
};
