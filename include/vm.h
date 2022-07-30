#pragma once

#include "chunk.h"
#include <memory>

enum class InterpretResult : std::uint8_t {
    Ok,
    CompileError,
    RuntimeError,
};

struct VM {
    [[nodiscard]] InterpretResult interpret(const Chunk& chunk) {
        m_chunk = std::make_unique<Chunk>(chunk);
        m_ip = m_chunk->code.cbegin();
        return run();
    }

private:
    [[nodiscard]] InterpretResult run() {
        const auto readByte = [this]() -> OpCode { return static_cast<OpCode>(*m_ip++); };
        const auto readConstant = [this, readByte]() { return m_chunk->constants[static_cast<std::size_t>(readByte())]; };
        while (true) {
#ifdef DEBUG_TRACE_EXECUTION
            const auto distance = std::distance(m_ip, m_chunk->code.cbegin());
            auto tmp = m_chunk->disassembleInstruction(static_cast<std::size_t>(distance));
#endif
            OpCode instruction;
            switch (instruction = readByte()) {
                using enum OpCode;
                case Return:
                    return InterpretResult::Ok;
                case Constant:
                    auto constant = readConstant();
                    fmt::print("{}\n", constant);
                    break;
            }
        }
    }
private:
    std::unique_ptr<Chunk> m_chunk;
    std::vector<std::uint8_t>::const_iterator m_ip;
};
