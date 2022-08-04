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
        for (auto dump = stack; not dump.empty(); dump.pop()) {
            fmt::print("[ {} ]", std::visit(PrintVisitor{}, dump.top()));
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
                if (not std::holds_alternative<Number>(last)) {
                    fmt::print(stderr, "Operand must be a number.");
                    return InterpretResult::RuntimeError;
                }
                const auto value = std::get<Number>(last);
                stack.pop();
                stack.push(-value);
                break;
            }
            case OpCode::Return: {
                fmt::print("{}\n", std::visit(PrintVisitor{}, stack.top()));
                stack.pop();
                return InterpretResult::Ok;
            }
        }
    }
}
