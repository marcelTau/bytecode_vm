#pragma once

#include <string_view>
#include <functional>
#include "chunk.h"
#include "scanner.h"

enum class Precedence : std::uint8_t {
    None,
    Assignment,  // =
    Or,          // or
    And,         // and
    Equality,    // == !=
    Comparison,  // < > <= >=
    Term,        // + -
    Factor,      // * /
    Unary,       // ! -
    Call,        // . ()
    Primary
};

using ParseFn = std::function<void()>;
struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

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
           /*TOKEN_LEFT_PAREN */   ParseRule {.prefix { std::bind(&Compiler::grouping, this) }, .infix {nullptr }, .precedence {   Precedence::None} },
           /*TOKEN_RIGHT_PAREN */  ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_LEFT_BRACE */   ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} }, 
           /*TOKEN_RIGHT_BRACE */  ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_COMMA */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_DOT */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_MINUS */        ParseRule {.prefix { std::bind(&Compiler::unary, this) }, .infix { std::bind(&Compiler::binary, this) }, .precedence { Precedence::Term} },
           /*TOKEN_PLUS */         ParseRule {.prefix { nullptr }, .infix { std::bind(&Compiler::binary, this) }, .precedence { Precedence::Term} },
           /*TOKEN_SEMICOLON */    ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_SLASH */        ParseRule {.prefix { nullptr }, .infix { std::bind(&Compiler::binary, this) }, .precedence { Precedence::Factor} },
           /*TOKEN_STAR */         ParseRule {.prefix { nullptr }, .infix { std::bind(&Compiler::binary, this) }, .precedence { Precedence::Factor} },
           /*TOKEN_BANG */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_BANG_EQUAL */   ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_EQUAL */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_EQUAL_EQUAL */  ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_GREATER */      ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_GREATER_EQUAL */ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_LESS */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_LESS_EQUAL */   ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_IDENTIFIER */   ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_STRING */       ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_NUMBER */       ParseRule {.prefix { std::bind(&Compiler::number, this) }, .infix {  nullptr }, .precedence { Precedence::None} },
           /*TOKEN_AND */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_CLASS */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_ELSE */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FALSE */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FOR */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FUN */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_IF */           ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_NIL */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_OR */           ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_PRINT */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_RETURN */       ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_SUPER */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_THIS */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_TRUE */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_VAR */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_WHILE */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_ERROR */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_EOF */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
        };
    }
    [[nodiscard]] bool compile(const std::string_view source);

private:
    void advance();
    void consume(TokenType type, const char *msg);
    void expression();

    void parsePrecedence(Precedence precedence);
    [[nodiscard]] ParseRule getRule(TokenType);
    void number();
    void grouping();
    void unary();
    void binary();

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
    std::array<ParseRule, 40> TokenTypeFunction;
    Scanner scanner;
    Parser parser;
    Chunk& chunk;
};
