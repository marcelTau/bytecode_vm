#include "vm.h"


InterpretResult VM::interpret(const std::string_view source) {
    Chunk chunk;

    Compiler compiler(chunk);

    if (not compiler.compile(source)) {
        return InterpretResult::CompileError;
    }

    m_chunk = std::make_unique<Chunk>(chunk);
    m_ip = m_chunk->code.begin();
    return run();
}

InterpretResult VM::run() {
    const auto readByte = [this]() -> OpCode { return static_cast<OpCode>(*m_ip++); };
    const auto readConstant = [this, readByte]() -> Value { return m_chunk->constants[static_cast<std::size_t>(readByte())]; };
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        const auto distance = std::distance(m_chunk->code.cbegin(), m_ip);
        fmt::print("          ");
        for (auto dump = m_stack; not dump.empty(); dump.pop()) {
            fmt::print("[ {} ]", std::visit(PrintVisitor{}, dump.top()));
        }
        fmt::print("\n");
        std::ignore = m_chunk->disassembleInstruction(static_cast<std::size_t>(distance));
#endif
        OpCode instruction;
        switch (instruction = readByte()) {
            case OpCode::Constant: {
                auto constant = readConstant();
                m_stack.push(constant);
                break;
            }
            case OpCode::Nil: m_stack.push(Nil{}); break;
            case OpCode::True: m_stack.push(true); break;
            case OpCode::False: m_stack.push(false); break;
            case OpCode::Add: { BINARY_OP(+); break; }
            case OpCode::Subtract: { BINARY_OP(-); break; }
            case OpCode::Multiply: { BINARY_OP(*); break; }
            case OpCode::Divide: { BINARY_OP(/); break; }
            case OpCode::Negate: {
                const auto last = m_stack.top();
                if (not std::holds_alternative<Number>(last)) {
                    fmt::print(stderr, "Operand must be a number.");
                    return InterpretResult::RuntimeError;
                }
                const auto value = std::get<Number>(last);
                m_stack.pop();
                m_stack.push(-value);
                break;
            }
            case OpCode::Return: {
                fmt::print("{}\n", std::visit(PrintVisitor{}, m_stack.top()));
                m_stack.pop();
                return InterpretResult::Ok;
            }
        }
    }
}

void VM::runtimeError(const char *msg) {
    fmt::print(stderr, "{}", msg);

    long instruction = std::distance(this->m_chunk->code.cbegin(), this->m_ip) - 1;
    std::size_t line = this->m_chunk->lines[static_cast<std::size_t>(instruction)];
    fmt::print(stderr, "[line {}] in script\n", line);
    resetStack();
}

void VM::resetStack() {
    m_stack = {};
}
