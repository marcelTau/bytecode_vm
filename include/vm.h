#pragma once

#include "chunk.h"
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

struct VM {
    [[nodiscard]] InterpretResult interpret(const Chunk& chunk) {
        m_chunk = std::make_unique<Chunk>(chunk);
        m_ip = m_chunk->code.cbegin();
        return run();
    }

private:
    [[nodiscard]] InterpretResult run() {
        const auto readByte = [this]() -> OpCode { return static_cast<OpCode>(*m_ip++); };
        const auto readConstant = [this, readByte]() -> Value { return m_chunk->constants[static_cast<std::size_t>(readByte())]; };
        while (true) {
#ifdef DEBUG_TRACE_EXECUTION
            const auto distance = std::distance(m_chunk->code.cbegin(), m_ip);
            fmt::print("          ");
            for (auto dump = stack; not dump.empty(); dump.pop()) {
                fmt::print("[ {} ]", dump.top());
            }
            fmt::print("\n");
            std::ignore = m_chunk->disassembleInstruction(static_cast<std::size_t>(distance));
#endif
            OpCode instruction;
            switch (instruction = readByte()) {
                case OpCode::Constant: {
                    auto constant = readConstant();
                    stack.push(constant);
                    break;
                }
                case OpCode::Add: { BINARY_OP(+); break; }
                case OpCode::Subtract: { BINARY_OP(-); break; }
                case OpCode::Multiply: { BINARY_OP(*); break; }
                case OpCode::Divide: { BINARY_OP(/); break; }
                case OpCode::Negate: {
                    const auto last = stack.top();
                    stack.pop();
                    stack.push(-last);
                    break;
                }
                case OpCode::Return: {
                    fmt::print("{}\n", stack.top());
                    stack.pop();
                    return InterpretResult::Ok;
                }
            }
        }
    }
private:
    std::unique_ptr<Chunk> m_chunk;
    std::vector<std::uint8_t>::const_iterator m_ip;
    std::stack<Value> stack;
};
