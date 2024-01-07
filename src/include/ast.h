#ifndef __AST_H
#define __AST_H

#define AST_NEW(arena, tag, ...) \
    ast_new(arena, (AST){tag, {.tag =(struct tag){__VA_ARGS__}}})

#include "arena.h"
#include "defines.h"
#include "string.h"
#include <stdio.h>

typedef struct AST AST;

struct AST {
    enum {
        AST_PROGRAM,
        
        AST_NUMBER,
        AST_STR,
        AST_IDENT,
        AST_BOOL,

        AST_EQ,
        AST_NEQ,
        AST_GT,
        AST_GTE,
        AST_LT,
        AST_LTE,
        AST_NOT,
        AST_AND,
        AST_OR,

        AST_ADD,
        AST_ADDEQ,
        AST_SUB,
        AST_SUBEQ,
        AST_MUL,
        AST_MULEQ,
        AST_DIV,
        AST_DIVEQ,
        AST_NEGATE,

        AST_LET,
        AST_PRINT,
    } tag;

    union {
        struct AST_PROGRAM 
        { AST* body[15]; u32 stmt_count;} AST_PROGRAM;

        struct AST_NUMBER 
        { f32 val; } AST_NUMBER;

        struct AST_STR 
        { String str; } AST_STR;

        struct AST_IDENT 
        { String ident; } AST_IDENT;

        struct AST_BOOL
        { bool val; } AST_BOOL;

        struct AST_EQ
        { AST* left; AST* right; } AST_EQ;

        struct AST_NEQ
        { AST* left; AST* right; } AST_NEQ;

        struct AST_GT
        { AST* left; AST* right; } AST_GT;

        struct AST_GTE
        { AST* left; AST* right; } AST_GTE;

        struct AST_LT
        { AST* left; AST* right; } AST_LT;

        struct AST_LTE
        { AST* left; AST* right; } AST_LTE;

        struct AST_NOT
        { AST* expr; } AST_NOT;
        
        struct AST_AND
        { AST* left; AST* right; } AST_AND;

        struct AST_OR
        { AST* left; AST* right; } AST_OR;

        struct AST_ADD 
        { AST* left; AST* right;} AST_ADD;

        struct AST_ADDEQ 
        { String ident; AST* expr;} AST_ADDEQ; 

        struct AST_SUB 
        { AST* left; AST* right;} AST_SUB;

        struct AST_SUBEQ 
        { String ident; AST* expr;} AST_SUBEQ; 

        struct AST_MUL 
        { AST* left; AST* right;} AST_MUL;

        struct AST_MULEQ 
        { String ident; AST* expr;} AST_MULEQ; 

        struct AST_DIV 
        { AST* left; AST* right;} AST_DIV;

        struct AST_DIVEQ 
        { String ident; AST* expr;} AST_DIVEQ; 

        struct AST_NEGATE 
        { AST* expr; } AST_NEGATE;

        struct AST_LET 
        { AST* expr; String ident; } AST_LET;

        struct AST_PRINT
        { AST* expr; } AST_PRINT;
    } data;
};

AST* ast_new(Arena* a, AST ast);
void ast_print(AST* ast);
void ast_emit(AST* ast, FILE* f);

#endif  //__AST_H
