#include "compiler.h"
#include "opcode.h"

bool Compiler::compile(const std::string_view source) {
    scanner = Scanner(source);

    parser.panicMode = false;
    parser.hadError = false;

    advance();
    expression();
    consume(TokenType::Eof, "Expect end of expression.");

    // @todo this is endCompiler()
    emitReturn();

    return !parser.hadError;
}

void Compiler::advance() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scanner.scanToken();

        if (parser.current.type != TokenType::Error) {
            break;
        }
        errorAtCurrent(parser.current.start);
    }
}

void Compiler::expression() {
}

void Compiler::number() {
    auto value = std::strtod(parser.previous.start, NULL);
    emitConstant(value);
}

void Compiler::consume(TokenType type, const char *msg) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(msg);
}

void Compiler::errorAtCurrent(const char *msg) {
    errorAt(parser.current, msg);
}

void Compiler::error(const char *msg) {
    errorAt(parser.previous, msg);
}

void Compiler::errorAt(Token& token, const char *msg) {
    if (parser.panicMode) {
        return;
    }
    parser.panicMode = true;
    fmt::print(stderr, "[line {}] Error", token.line);

    if (token.type == TokenType::Eof) {
        fmt::print(stderr, " at end");
    } else if (token.type == TokenType::Error) {
        // ignore that case
    } else {
        fmt::print(stderr, " at '{:.{}}'", token.start, token.length);
    }

    fmt::print(stderr, ": {}\n", msg);
    parser.hadError = true;
}

void Compiler::emitReturn() {
    emitByte(OpCode::Return);
}

void Compiler::emitConstant(Value value) {
    emitBytes(OpCode::Constant, makeConstant(value));
}

std::uint8_t Compiler::makeConstant(Value value) {
    auto constant = chunk.addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return static_cast<std::uint8_t>(constant);
}















