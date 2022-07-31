#pragma once

#include <string_view>
#include "chunk.h"
#include "scanner.h"

struct Parser {
    Parser() = default;
    Token current;
    Token previous;
    bool hadError { false };
    bool panicMode { false };
};

// big overhead but interesting
//template <typename opcode>
//concept IsOpcode = requires(opcode t)
//{
    //{ static_cast<std::uint8_t>(t) } -> std::convertible_to<std::uint8_t>; 
//};

template <typename opcode>
concept IsOpcode = std::is_same_v<opcode, std::uint8_t> || std::is_same_v<opcode, OpCode>;

struct Compiler {
    Compiler(Chunk& chunk) : chunk(chunk) {}
    [[nodiscard]] bool compile(const std::string_view source);

private:
    void advance();
    void consume(TokenType type, const char *msg);

    template<typename opcode>
    requires IsOpcode<opcode>
    void emitByte(opcode byte) {
        chunk.push(byte, parser.previous.line);
    }

    template<typename opcode>
    requires IsOpcode<opcode>
    void emitBytes(opcode byte1, opcode byte2) {
        emitByte(byte1);
        emitByte(byte2);
    }


    void emitReturn();

    void errorAtCurrent(const char *msg);
    void error(const char *msg);
    void errorAt(Token& token, const char *msg);

private:
    Scanner scanner;
    Parser parser;
    Chunk& chunk;
};
