#include "compiler.h"
#include "opcode.h"
#include <cassert>
#include <iostream>

bool Compiler::compile(const std::string_view source) {
    scanner = Scanner(source.data());

    parser.panicMode = false;
    parser.hadError = false;

    advance();
    expression();
    consume(TokenType::Eof, "Expect end of expression.");

    // @todo this is endCompiler()
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (not parser.hadError) {
        chunk.disassembleChunk("code");
    }
#endif

    return not parser.hadError;
}

void Compiler::advance() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scanner.scanToken();

#ifdef DEBUG_PRINT_CODE
        fmt::print("ADVANCE: {}\n", parser.current.type);
#endif

        if (parser.current.type != TokenType::Error) {
            break;
        }
        errorAtCurrent(parser.current.start);
    }
}

void Compiler::expression() {
    parsePrecedence(Precedence::Assignment);
}

void Compiler::number() {
    auto value = std::strtod(parser.previous.start, NULL);
    emitConstant(value);
}

void Compiler::grouping() {
    expression();
    consume(TokenType::RightParen, "Expect ')' after expression.");
}

void Compiler::unary() {
    auto operatorType = parser.previous.type;

    parsePrecedence(Precedence::Unary);

    switch (operatorType) {
        case TokenType::Minus: return emitByte(OpCode::Negate);
        default: assert(false && "Unreachable TokenType in unary expression.");
    }
}

void Compiler::binary() {
    auto operatorType = parser.previous.type;
    auto rule = getRule(operatorType);


    auto newPrecedence = static_cast<std::uint8_t>(rule.precedence) + 1;
    fmt::print("Precedence of {}: {} -> {}", operatorType, (int)rule.precedence, newPrecedence);
    parsePrecedence(static_cast<Precedence>(newPrecedence));

    switch (operatorType) {
        case TokenType::Plus: return emitByte(OpCode::Add);
        case TokenType::Minus: return emitByte(OpCode::Subtract);
        case TokenType::Star: return emitByte(OpCode::Multiply);
        case TokenType::Slash: return emitByte(OpCode::Divide);
        default: assert(false && "Unreachable TokenType in binary expression.");
    }
}

void Compiler::parsePrecedence(Precedence precedence) {
    advance();

    auto prefixRule = getRule(parser.previous.type).prefix;

    if (not prefixRule) {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type).precedence) {
        advance();
        auto infixRule = getRule(parser.previous.type).infix;
        infixRule();
    }
}

ParseRule Compiler::getRule(TokenType type) {
    return TokenTypeFunction[static_cast<std::uint8_t>(type)];
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















