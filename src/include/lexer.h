#ifndef __LEXER_H
#define __LEXER_H

#include "arena.h"
#include "string.h"

typedef enum {
    Token_None,

    Token_Ident, Token_Number, Token_String,

    Token_Plus, Token_Dash, Token_Star, Token_Slash,
    Token_Comma, Token_Dot, Token_Semicolon, Token_Greater,
    Token_Less, Token_Eq, Token_Bang,

    Token_LParen, Token_RParen,
    Token_RCurly, Token_LCurly,
    Token_LBrace, Token_RBrace,

    Token_DoubleEq, Token_BangEquals, Token_DoubleColon, 
    Token_GreaterEq, Token_LessEq,

    Token_If, Token_Else, Token_Elif, Token_For,
    Token_While, Token_Colon, Token_At,
    Token_Func, Token_Class, Token_Let, Token_True,
    Token_False, Token_And, Token_Or, 

    Token_EOF,
    Token_Unexpected,
    TokenTypeCount,
} TokenType;

typedef struct token_t {
    TokenType type;
    String lexeme;
} Token;

#define token(type) token_new(type, string(""))

typedef struct lexer_t {
    Arena* arena;
    String src;
    u64 cursor;
    u64 line_number;
    u64 column;
} Lexer;

Token token_new(TokenType type, String lexeme);
void token_print(Token t);
String token_type_str(TokenType type);

Lexer* lexer_new(Arena* a, String src);

Token lexer_next_token(Lexer* lexer);

#endif  //__LEXER_H
