#include <fmt/format.h>
#include <iostream>
#include "header.h"

#include "chunk.h"
#include "vm.h"

int main() {
#ifdef DEBUG_TRACE_EXECUTION
    fmt::print("===== DEBUG MODE =====\n");
#endif


    Chunk c;

    std::size_t constant = c.addConstant(1.2);
    c.push(OpCode::Constant, 123);
    c.push(static_cast<std::uint8_t>(constant), 123);


    constant = c.addConstant(2.2);
    c.push(OpCode::Constant, 123);
    c.push(static_cast<std::uint8_t>(constant), 123);

    c.push(OpCode::Add, 123);

    c.push(OpCode::Negate, 123);
    c.push(OpCode::Return, 123);

    c.disassembleChunk("test chunk");

    VM vm;

    std::ignore = vm.interpret(c);

    return 0;
}
