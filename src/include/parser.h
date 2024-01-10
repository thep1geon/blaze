#ifndef __PARSER_H
#define __PARSER_H

#include "hashmap.h"
#include "lexer.h"
#include "ast.h"

typedef enum {
    Precedence_Min,
    Precedence_Or,
    Precedence_And,
    Precedence_Not,
    Precedence_Cmp,
    Precedence_EqCmp,
    Precedence_Term,
    Precedence_Factor,
} Precedence;

typedef struct parser_t {
    AST*   ast;
    Arena* arena;
    Lexer* lexer;   
    HashMap* var_map;

    Token prev;
    Token curr;
    Token next;
} Parser;

Parser* parser_new(Lexer* lexer);
void    parser_free(Parser* p);
void    parser_advance(Parser* p);
void    parser_parse(Parser* p);

#endif  //__PARSER_H
