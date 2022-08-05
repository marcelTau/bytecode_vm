#include "compiler.h"
#include "opcode.h"
#include <cassert>
#include <iostream>

bool Compiler::compile(const std::string_view source) {
    scanner = Scanner(source.data());

    parser.panicMode = false;
    parser.hadError = false;

    advance();

    while (not match(TokenType::Eof)) {
        declaration();
    }

    // @todo this is endCompiler()
    emitReturn();
#ifdef DEBUG_PRINT_CODE
if (not parser.hadError) {
    chunk.disassembleChunk("code");
}
#endif

    return not parser.hadError;
}

void Compiler::declaration() {
    if (match(TokenType::Var)) {
        varDeclaration();
    } else {
        statement();
    }

    if (parser.panicMode) {
        synchronize();
    }
}

void Compiler::statement() {
    if (match(TokenType::Print)) {
        printStatement();
    } else {
        expressionStatement();
    }
}

void Compiler::printStatement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    emitByte(OpCode::Print);
}

void Compiler::expressionStatement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    emitByte(OpCode::Pop);
}

void Compiler::varDeclaration() {
    std::uint8_t global = parseVariable("Expect variable name.");

    if (match(TokenType::Equal)) {
        expression();
    } else {
        emitByte(OpCode::Nil);
    }

    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");

    defineVariable(global);
}

std::uint8_t Compiler::parseVariable(const char *errorMessage) {
    consume(TokenType::Identifier, errorMessage);
    return identifierConstant(parser.previous);
}

std::uint8_t Compiler::identifierConstant(const Token& name) {
    return makeConstant(std::string(name.start, name.length));
}

void Compiler::defineVariable(std::uint8_t global) {
    emitBytes(OpCode::DefineGlobal, global);
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
    parsePrecedence(Precedence::Assignment);
}

void Compiler::number(bool) {
    auto value = std::strtod(parser.previous.start, NULL);
    emitConstant(value);
}

void Compiler::grouping(bool) {
    expression();
    consume(TokenType::RightParen, "Expect ')' after expression.");
}

void Compiler::unary(bool) {
    auto operatorType = parser.previous.type;

    parsePrecedence(Precedence::Unary);

    switch (operatorType) {
        case TokenType::Minus: return emitByte(OpCode::Negate);
        case TokenType::Bang: return emitByte(OpCode::Not);
        default: assert(false && "Unreachable TokenType in unary expression.");
    }
}

void Compiler::binary(bool) {
    auto operatorType = parser.previous.type;
    auto rule = getRule(operatorType);


    auto newPrecedence = static_cast<std::uint8_t>(rule.precedence) + 1;
    parsePrecedence(static_cast<Precedence>(newPrecedence));

    switch (operatorType) {
        case TokenType::Plus: return emitByte(OpCode::Add);
        case TokenType::Minus: return emitByte(OpCode::Subtract);
        case TokenType::Star: return emitByte(OpCode::Multiply);
        case TokenType::Slash: return emitByte(OpCode::Divide);
        case TokenType::BangEqual: return emitBytes(OpCode::Equal, OpCode::Not);
        case TokenType::EqualEqual: return emitByte(OpCode::Equal);
        case TokenType::Greater: return emitByte(OpCode::Greater);
        case TokenType::GreaterEqual: return emitBytes(OpCode::Less, OpCode::Not);
        case TokenType::Less: return emitByte(OpCode::Less);
        case TokenType::LessEqual: return emitBytes(OpCode::Greater, OpCode::Not);
        default: assert(false && "Unreachable TokenType in binary expression.");
    }
}

void Compiler::literal(bool) {
    switch (parser.previous.type) {
        case TokenType::False: return emitByte(OpCode::False);
        case TokenType::Nil: return emitByte(OpCode::Nil);
        case TokenType::True: return emitByte(OpCode::True);
        default: assert(false && "Unreachable TokenType in literal expression.");
    }
}

void Compiler::string(bool) {
    std::string s(parser.previous.start + 1, parser.previous.length - 2);
    emitConstant(s);
}

void Compiler::variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

void Compiler::namedVariable(const Token& name, bool canAssign) {
    std::uint8_t arg = identifierConstant(name);

    if (canAssign && match(TokenType::Equal)) {
        expression();
        emitBytes(OpCode::SetGlobal, arg);
    } else {
        emitBytes(OpCode::GetGlobal, arg);
    }
}

void Compiler::parsePrecedence(Precedence precedence) {
    advance();

    auto prefixRule = getRule(parser.previous.type).prefix;

    if (not prefixRule) {
        error("Expect expression.");
        return;
    }

    bool canAssign = precedence <= Precedence::Assignment;
    prefixRule(canAssign);

    while (precedence <= getRule(parser.current.type).precedence) {
        advance();
        auto infixRule = getRule(parser.previous.type).infix;
        infixRule(canAssign);
    }

    if (canAssign && match(TokenType::Equal)) {
        error("Invalid assignment target.");
    }
}

ParseRule Compiler::getRule(TokenType type) {
    return TokenTypeFunction[static_cast<std::uint8_t>(type)];
}

bool Compiler::check(TokenType type) {
    return parser.current.type == type;
}

bool Compiler::match(TokenType type) {
    if (not check(type)) {
        return false;
    }
    advance();
    return true;
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

void Compiler::emitConstant(const Value& value) {
    emitBytes(OpCode::Constant, makeConstant(value));
}

std::uint8_t Compiler::makeConstant(const Value& value) {
    auto constant = chunk.addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return static_cast<std::uint8_t>(constant);
}

void Compiler::synchronize() {
    parser.panicMode = false;

    while (parser.current.type != TokenType::Eof) {
        if (parser.previous.type == TokenType::Semicolon) {
            return;
        }
        switch (parser.current.type) {
            case TokenType::Class:
            case TokenType::Fun:
            case TokenType::Var:
            case TokenType::For:
            case TokenType::If:
            case TokenType::While:
            case TokenType::Print:
            case TokenType::Return:
                return;
            default: /*nothing*/;
        }
        advance();
    }
}













