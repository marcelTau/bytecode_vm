#pragma once

#include <string_view>
#include <functional>
#include "chunk.h"
#include "scanner.h"

struct Parser {
    Parser() = default;
    Token current;
    Token previous;
    bool hadError { false };
    bool panicMode { false };
};

template <typename opcode>
concept IsOpcode = std::is_same_v<opcode, std::uint8_t> || std::is_same_v<opcode, OpCode>;

struct Compiler {

    Compiler(Chunk& chunk) : chunk(chunk) {
        TokenTypeFunction = {
            std::bind(&Compiler::number, this)
        };
    }

    [[nodiscard]] bool compile(const std::string_view source);


private:
    void advance();
    void consume(TokenType type, const char *msg);
    void expression();

    void number();

    template<typename opcode>
    requires IsOpcode<opcode>
    void emitByte(opcode byte) {
        chunk.push(byte, parser.previous.line);
    }

    template<typename opcode, typename opcode2>
    requires IsOpcode<opcode> && IsOpcode<opcode2>
    void emitBytes(opcode byte1, opcode2 byte2) {
        emitByte(byte1);
        emitByte(byte2);
    }

    void emitConstant(Value value);
    void emitReturn();

    std::uint8_t makeConstant(Value value);

    void errorAtCurrent(const char *msg);
    void error(const char *msg);
    void errorAt(Token& token, const char *msg);

private:
    std::array<std::function<void()>, 1> TokenTypeFunction;
    Scanner scanner;
    Parser parser;
    Chunk& chunk;
};
