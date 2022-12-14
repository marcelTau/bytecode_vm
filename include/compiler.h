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

using ParseFn = std::function<void(bool)>;
struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

struct Parser {
    Parser() = default;
    Token current {};
    Token previous {};
    bool hadError { false };
    bool panicMode { false };
};

// user defined literals turns a ULL into u8
inline constexpr std::uint8_t operator""_u8(unsigned long long arg) noexcept { return static_cast<std::uint8_t>(arg); }

template <typename opcode>
concept IsOpcode = std::is_same_v<opcode, std::uint8_t> || std::is_same_v<opcode, OpCode>;


#define BIND(func_name) std::bind(&Compiler::func_name, this, std::placeholders::_1)

struct Compiler {
    Compiler(Chunk& chunk) : chunk(chunk) {
        TokenTypeFunction = {
           /*TOKEN_LEFT_PAREN */   ParseRule {.prefix { BIND(grouping) }, .infix { nullptr }, .precedence {   Precedence::None} },
           /*TOKEN_RIGHT_PAREN */  ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_LEFT_BRACE */   ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} }, 
           /*TOKEN_RIGHT_BRACE */  ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_COMMA */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_DOT */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_MINUS */        ParseRule {.prefix { BIND(unary) }, .infix { BIND(binary) }, .precedence { Precedence::Term} },
           /*TOKEN_PLUS */         ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Term} },
           /*TOKEN_SEMICOLON */    ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_SLASH */        ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Factor} },
           /*TOKEN_STAR */         ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Factor} },
           /*TOKEN_BANG */         ParseRule {.prefix { BIND(unary) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_BANG_EQUAL */   ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Equality} },
           /*TOKEN_EQUAL */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_EQUAL_EQUAL */  ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Equality} },
           /*TOKEN_GREATER */      ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Comparison} },
           /*TOKEN_GREATER_EQUAL */ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Comparison} },
           /*TOKEN_LESS */         ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Comparison} },
           /*TOKEN_LESS_EQUAL */   ParseRule {.prefix { nullptr }, .infix { BIND(binary) }, .precedence { Precedence::Comparison} },
           /*TOKEN_IDENTIFIER */   ParseRule {.prefix { BIND(variable) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_STRING */       ParseRule {.prefix { BIND(string) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_NUMBER */       ParseRule {.prefix { BIND(number) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_AND */          ParseRule {.prefix { nullptr }, .infix { BIND(and_) }, .precedence { Precedence::And} },
           /*TOKEN_CLASS */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_ELSE */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FALSE */        ParseRule {.prefix { BIND(literal) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FOR */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_FUN */          ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_IF */           ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_NIL */          ParseRule {.prefix { BIND(literal) }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_OR */           ParseRule {.prefix { nullptr }, .infix { BIND(or_) }, .precedence { Precedence::Or} },
           /*TOKEN_PRINT */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_RETURN */       ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_SUPER */        ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_THIS */         ParseRule {.prefix { nullptr }, .infix { nullptr }, .precedence { Precedence::None} },
           /*TOKEN_TRUE */         ParseRule {.prefix { BIND(literal) }, .infix { nullptr }, .precedence { Precedence::None} },
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
    void declaration();
    void statement();
    void block();

    void whileStatement();
    void ifStatement();
    void printStatement();
    void expressionStatement();
    void varDeclaration();
    void declareVariable();
    void addLocal(const Token& name);


    void parsePrecedence(Precedence precedence);

    [[nodiscard]] bool identifiersEqual(const Token& lhs, const Token& rhs);
    [[nodiscard]] std::uint8_t parseVariable(const char *errorMessage);
    [[nodiscard]] std::uint8_t identifierConstant(const Token& name);
    void defineVariable(std::uint8_t global);
    void patchJump(int offset);

    [[nodiscard]] ParseRule getRule(TokenType type);
    [[nodiscard]] bool match(TokenType type);
    [[nodiscard]] bool check(TokenType type);

    void number(bool);
    void grouping(bool);
    void unary(bool);
    void binary(bool);
    void literal(bool);
    void string(bool);
    void variable(bool canAssign);
    void namedVariable(const Token& name, bool canAssign);
    void markInitialized();
    [[nodiscard]] int resolveLocal(const Token& name);
    void and_(bool);
    void or_(bool);

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

    template<typename opcode>
    requires IsOpcode<opcode>
    int emitJump(opcode instruction) {
        emitByte(instruction);
        emitByte(0xff_u8);
        emitByte(0xff_u8);
        return static_cast<int>(chunk.code.size()) - 2;
    }

    void emitConstant(const Value& value);
    void emitReturn();
    void emitLoop(std::size_t loopStart);

    std::uint8_t makeConstant(const Value& value);

    void errorAtCurrent(const char *msg);
    void error(const char *msg);
    void errorAt(Token& token, const char *msg);
    void synchronize();
    void beginScope();
    void endScope();

private:
    struct Local {
        Token name;
        int depth;
    };
    struct Variables {
        std::array<Local, UINT8_MAX + 1> locals;
        int localCount { 0 };
        int scopeDepth { 0 };
    } variables;
    std::array<ParseRule, 40> TokenTypeFunction;
    Scanner scanner;
    Parser parser;
    Chunk& chunk;
};
