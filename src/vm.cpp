#include "vm.h"
#include <cassert>


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
    const auto readShort = [this]() -> std::uint16_t { 
        m_ip += 2; 
        return static_cast<std::uint16_t>((m_ip[-2] << 8) | m_ip[-1]);
    };
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        const auto distance = std::distance(m_chunk->code.cbegin(), m_ip);
        fmt::print("          ");
        for (auto dump = m_stack; not dump.empty(); dump.pop_back()) {
            fmt::print("[ {} ]", std::visit(PrintVisitor{}, dump.back()));
        }
        fmt::print("\n");
        std::ignore = m_chunk->disassembleInstruction(static_cast<std::size_t>(distance));
#endif
        OpCode instruction;
        switch (instruction = readByte()) {
            case OpCode::Constant: {
                auto constant = readConstant();
                m_stack.push_back(constant);
                break;
            }
            case OpCode::Nil: { m_stack.emplace_back(Nil{}); break; };
            case OpCode::True: { m_stack.emplace_back(true); break; };
            case OpCode::False: { m_stack.emplace_back(false); break; };
            case OpCode::Pop: { std::ignore = pop(); break; };
            case OpCode::GetLocal: {
                auto slot = readByte();
                m_stack.push_back(m_stack[static_cast<std::size_t>(slot)]);
                break;
            };
            case OpCode::SetLocal: {
                auto slot = readByte();
                m_stack[static_cast<std::size_t>(slot)] = peek();
                break;
            };
            case OpCode::GetGlobal: {
                const auto name = get_objtype_unchecked<std::string>(readConstant());
                Value value;
                try {
                    value = globals.at(name);
                } catch (const std::exception&) {
                    runtimeError(fmt::format("Undefined variable '{}'", name));
                    return InterpretResult::RuntimeError;
                }
                m_stack.push_back(value);
                break;
            };
            case OpCode::DefineGlobal: {
                const auto name = get_objtype_unchecked<std::string>(readConstant());
                globals[name] = peek();
                std::ignore = pop();
                break;
            };
            case OpCode::SetGlobal: {
                const auto name = get_objtype_unchecked<std::string>(readConstant());
                try {
                    // check if this key already exists
                    globals.at(name);
                    globals[name] = peek();
                } catch (const std::exception&) {
                    runtimeError(fmt::format("Undefined variable '{}'", name));
                    return InterpretResult::RuntimeError;
                }
                break;
                
            }
            case OpCode::Equal: {
                const auto b = pop();
                const auto a = pop();
                m_stack.emplace_back(valuesEqual(a, b));
                break;
            };
            case OpCode::Greater: { BINARY_OP(>); break; };
            case OpCode::Less: { BINARY_OP(<); break; };
            case OpCode::Add: { 
                if (holds_obj_type<std::string>(peek()) && holds_obj_type<std::string>(peek(1))) {
                    concatenate();
                } else {
                    BINARY_OP(+); 
                }
                break; 
            };
            case OpCode::Subtract: { BINARY_OP(-); break; }
            case OpCode::Multiply: { BINARY_OP(*); break; }
            case OpCode::Divide: { BINARY_OP(/); break; }
            case OpCode::Not: {
                const auto value = m_stack.back();
                m_stack.pop_back();
                const bool truthyness = isFalsey(value);
                m_stack.emplace_back(truthyness);
                break;
            };
            case OpCode::Negate: {
                const auto last = m_stack.back();
                if (not std::holds_alternative<Number>(last)) {
                    fmt::print(stderr, "Operand must be a number.");
                    return InterpretResult::RuntimeError;
                }
                const auto value = std::get<Number>(last);
                m_stack.pop_back();
                m_stack.emplace_back(-value);
                break;
            };
            case OpCode::Jump: {
                std::uint16_t offset = readShort();
                m_ip += offset;
                break;
            };
            case OpCode::JumpIfFalse: {
                std::uint16_t offset = readShort();

                if (isFalsey(peek(0))) {
                    m_ip += offset;
                }
                break;
            };
            case OpCode::Print: {
                fmt::print("{}\n", std::visit(PrintVisitor{}, pop()));
                break;
            };
            case OpCode::Return: {
                return InterpretResult::Ok;
            }
        }
    }
}

void VM::runtimeError(const std::string& msg) {
    fmt::print(stderr, "{}", msg);

    long instruction = std::distance(this->m_chunk->code.cbegin(), this->m_ip) - 1;
    std::size_t line = this->m_chunk->lines[static_cast<std::size_t>(instruction)];
    fmt::print(stderr, "[line {}] in script\n", line);
    resetStack();
}

void VM::resetStack() {
    m_stack = {};
}

void VM::concatenate() {
    // unchecked should be fine, since we're checking the types before in run()
    auto b = get_objtype_unchecked<std::string>(pop());
    auto a = get_objtype_unchecked<std::string>(pop());
    m_stack.emplace_back(a + b);
}

bool VM::isFalsey(const Value& value) {
    return (
        std::holds_alternative<Nil>(value) ||
        (std::holds_alternative<Bool>(value) && std::get<Bool>(value) == false)
    );
}

bool VM::valuesEqual(const Value& a, const Value& b) {
    return std::visit(EqualityVisitor{}, a, b);
}

Value VM::peek(std::size_t many) {
    if (many == 0) {
        return m_stack.back();
    } else if (many == 1) {
        auto tmpValue = pop();
        auto retValue = m_stack.back();
        m_stack.push_back(tmpValue);
        return retValue;
    }
    assert(false && "peek is not implemented for numbers > 1.");
}

Value VM::pop() {
    auto v = m_stack.back();
    m_stack.pop_back();
    return v;
}





















