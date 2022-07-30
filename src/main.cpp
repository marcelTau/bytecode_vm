#include <fmt/format.h>
#include <iostream>
#include "header.h"

#include "chunk.h"

int main() {
    Chunk c;

    std::size_t constant = c.addConstant(1.2);
    c.push(OpCode::Constant, 123);
    c.push(static_cast<std::uint8_t>(constant), 123);
    c.push(OpCode::Return, 123);

    c.disassembleChunk("test chunk");

    return 0;
}
