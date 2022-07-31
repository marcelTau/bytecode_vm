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

#define BINARY_OP(op) \
    do { \
      auto b = stack.top(); \
      stack.pop(); \
      auto a = stack.top(); \
      stack.pop(); \
      stack.push(a op b); \
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
