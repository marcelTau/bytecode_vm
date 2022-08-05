#include "chunk.h"

void Chunk::push(OpCode opcode, std::size_t line) {
    code.push_back(static_cast<std::uint8_t>(opcode));
    lines.push_back(line);
}

void Chunk::push(std::uint8_t opcode, std::size_t line) {
    code.push_back(opcode);
    lines.push_back(line);
}

void Chunk::disassembleChunk(const std::string_view name) const {
    fmt::print("== {} ({}) ==\n", name, code.size());

    for (std::size_t offset = 0; offset < code.size();) {
        offset = disassembleInstruction(offset);
    }
}

std::size_t Chunk::disassembleInstruction(std::size_t offset) const {
    fmt::print("{:04d} ", offset);

    if (offset > 0 && lines[offset] == lines[offset - 1]) {
        fmt::print("   | ");
    } else {
        fmt::print("{:4d} ", lines[offset]);
    }

    const auto instruction = static_cast<OpCode>(code[offset]);

    switch (instruction) {
        case OpCode::Constant: return constantInstruction("Constant", offset);
        case OpCode::Nil: return simpleInstruction("Nil", offset);
        case OpCode::True: return simpleInstruction("True", offset);
        case OpCode::False: return simpleInstruction("False", offset);
        case OpCode::Equal: return simpleInstruction("Equal", offset);
        case OpCode::Greater: return simpleInstruction("Greater", offset);
        case OpCode::Less: return simpleInstruction("Less", offset);
        case OpCode::Add: return simpleInstruction("Add", offset);
        case OpCode::Subtract: return simpleInstruction("Subtract", offset);
        case OpCode::Multiply: return simpleInstruction("Multiply", offset);
        case OpCode::Divide: return simpleInstruction("Divide", offset);
        case OpCode::Not: return simpleInstruction("Not", offset);
        case OpCode::Negate: return simpleInstruction("Negate", offset);
        case OpCode::Return: return simpleInstruction("Return", offset);
        default:
            fmt::print("Unknown opcode {}\n", instruction);
            return offset + 1;
    }
}

std::size_t Chunk::simpleInstruction(const std::string_view name, std::size_t offset) const {
    fmt::print("{}\n", name);
    return offset + 1;
}

std::size_t Chunk::addConstant(const Value& value) {
    constants.push_back(value);
    return constants.size() - 1;
}

std::size_t Chunk::constantInstruction(const std::string_view name, std::size_t offset) const {
    std::uint8_t constant = code[offset + 1];
    auto variant = constants[constant];
    fmt::print("{:16} {:4d} '{}'\n", name, constant, std::visit(PrintVisitor{}, variant));
    return offset + 2;
}
