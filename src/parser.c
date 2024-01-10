#include "include/parser.h"
#include "include/arena.h"
#include "include/ast.h"
#include "include/hashmap.h"
#include "include/lexer.h"
#include "include/string.h"
#include "include/err.h"
#include <assert.h>
#include <stdlib.h>

static u64 err_line = 0;
static u64 err_col = 0;

#define ParserErr(parser, token, msg)do {\
err_line = token.line;\
err_col = token.col;\
parser_free(parser);\
err(msg, err_line, err_col);\
} while (0)

static Precedence precedence_lookup[TokenTypeCount] = {
    [Token_Or] = Precedence_Or,
    [Token_And] = Precedence_And,
    [Token_NotEq] = Precedence_EqCmp,
    [Token_DoubleEq] = Precedence_EqCmp,
    [Token_Bang] = Precedence_Not,
    [Token_Greater] = Precedence_Cmp,
    [Token_GreaterEq] = Precedence_Cmp,
    [Token_Less] = Precedence_Cmp,
    [Token_LessEq] = Precedence_Cmp,
    [Token_Plus] = Precedence_Term,
    [Token_Dash] = Precedence_Term,
    [Token_Star] = Precedence_Factor,
    [Token_Slash] = Precedence_Factor,
};

static AST* parse_stmt(Parser* p);
static AST* parse_expr(Parser* p, Precedence prev_prec);
static AST* parse_infix_expr(Parser* p, Token operator, AST* left);
static AST* parse_number(Parser* p);
static AST* parse_terminal_expr(Parser* p);

Parser* parser_new(Lexer* lexer) {
    Parser* p = arena_alloc(lexer->arena, sizeof(Parser));
    p->arena = lexer->arena;
    p->lexer = lexer;

    AST ast = {.tag=AST_PROGRAM, .data.AST_PROGRAM={.stmt_count=0}};
    p->ast = ast_new(p->arena, ast);

    p->var_map = hashmap_new(p->arena);
    
    parser_advance(p);
    parser_advance(p);

    return p;
}

void parser_free(Parser* p) {
    arena_free(p->arena);
}

static AST* parse_stmt(Parser* p) {
    AST* stmt = arena_alloc(p->arena, sizeof(AST));

    if (p->curr.type == Token_Let) {
        parser_advance(p); 

        if (p->curr.type == Token_Ident && p->next.type == Token_Eq) {
            stmt->tag = AST_LET;
            struct AST_LET* data = &stmt->data.AST_LET;
            data->ident = p->curr.lexeme;

            parser_advance(p); 
            parser_advance(p); 

            VarType type = TypeNil;
            switch (p->curr.type) {
                case Token_Nil: break;
                case Token_String: type = TypeStr; break;
                case Token_Number: type = TypeNum; break;
                case Token_True:
                case Token_False: type = TypeBool; break;
                default: break;
            }

            hashmap_insert(p->var_map, data->ident, type);
                
            data->expr = parse_expr(p, Precedence_Min);
        } 
    } 
    else if (p->curr.type == Token_Ident) {
        if (string_eq(p->curr.lexeme, string("print"))) {
            stmt->tag = AST_PRINT;

            parser_advance(p);

            stmt->data.AST_PRINT.expr = parse_expr(p, Precedence_Min);
        }
        else {
            switch (p->next.type) {
                case Token_PlusEq: {
                    parser_advance(p);
                    stmt->data.AST_ADDEQ.ident = p->prev.lexeme;

                    parser_advance(p);
                    stmt->tag = AST_ADDEQ;
                    stmt->data.AST_ADDEQ.expr = parse_expr(p, Precedence_Min);
                    break;
                }
                case Token_MinusEq: {
                    parser_advance(p);
                    stmt->data.AST_SUBEQ.ident = p->prev.lexeme;

                    parser_advance(p);
                    stmt->tag = AST_SUBEQ;
                    stmt->data.AST_SUBEQ.expr = parse_expr(p, Precedence_Min);
                    break;
                }
                case Token_MultEq: {
                    parser_advance(p);
                    stmt->data.AST_MULEQ.ident = p->prev.lexeme;

                    parser_advance(p);
                    stmt->tag = AST_MULEQ;
                    stmt->data.AST_MULEQ.expr = parse_expr(p, Precedence_Min);
                    break;
                }
                case Token_DivEq: {
                    parser_advance(p);
                    stmt->data.AST_MULEQ.ident = p->prev.lexeme;

                    parser_advance(p);
                    stmt->tag = AST_DIVEQ;
                    stmt->data.AST_DIVEQ.expr = parse_expr(p, Precedence_Min);
                    break;
                }
                default: token_print(p->curr); ParserErr(p, p->curr, "Not Implemented #0");
            } 
        }
    }
    else {
        return parse_expr(p, Precedence_Min);
    }

    return stmt;
}

static AST* parse_number(Parser* p) {
    AST* num = AST_NEW(p->arena, AST_NUMBER, atof(p->curr.lexeme.data));
    parser_advance(p);
    return num;
}

static AST* parse_terminal_expr(Parser* p) {
    AST* ret = 0;
    switch (p->curr.type) {
        case Token_Number: {
            ret = parse_number(p);
            break;
        }
        case Token_LParen: {
            parser_advance(p);
            ret = parse_expr(p, Precedence_Min);
            if (p->curr.type == Token_RParen) {
                parser_advance(p);
            }
            break;
        }
        case Token_Dash: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_NEGATE, parse_terminal_expr(p));
            break;
        }
        case Token_Bang: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_NOT, parse_terminal_expr(p));
            break;
        }
        case Token_Nil: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_NIL, 0);
            break;
        }
        case Token_String: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_STR, p->prev.lexeme);
            break;
        }
        case Token_Ident: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_IDENT, p->prev.lexeme);
            break;
        }
        case Token_True: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_BOOL, 1);
            break;
        }
        case Token_False: {
            parser_advance(p);
            ret = AST_NEW(p->arena, AST_BOOL, 0);
            break;
        }
        default: token_loc_print(p->curr); assert(0 && "Unknown Terminal Expr");
    }

    return ret;
}

static AST* parse_infix_expr(Parser* p, Token op, AST* left) {
    AST* node = arena_alloc(p->arena, sizeof(AST));
    switch (op.type) {
        case Token_Plus:{
            node->tag = AST_ADD;
            node->data.AST_ADD.left = left;
            node->data.AST_ADD.right = parse_expr(p, 
                                        precedence_lookup[op.type]);
            return node;
        }
        case Token_Dash:{
            node->tag = AST_SUB;
            node->data.AST_SUB.left = left;
            node->data.AST_SUB.right = parse_expr(p,
                                        precedence_lookup[op.type]);
            return node;
        }
        case Token_Star:{
            node->tag = AST_MUL;
            node->data.AST_MUL.left = left;
            node->data.AST_MUL.right = parse_expr(p, 
                                        precedence_lookup[op.type]);
            return node;
        }
        case Token_Slash:{
            node->tag = AST_DIV;
            node->data.AST_DIV.left = left;
            node->data.AST_DIV.right = parse_expr(p, 
                                        precedence_lookup[op.type]);
            return node;
        }
        case Token_DoubleEq: {
            node->tag = AST_EQ;
            node->data.AST_EQ.left = left;
            node->data.AST_EQ.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_NotEq: {
            node->tag = AST_NEQ;
            node->data.AST_NEQ.left = left;
            node->data.AST_NEQ.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_Greater: {
            node->tag = AST_GT;
            node->data.AST_GT.left = left;
            node->data.AST_GT.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_GreaterEq: {
            node->tag = AST_GTE;
            node->data.AST_GTE.left = left;
            node->data.AST_GTE.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_Less: {
            node->tag = AST_LT;
            node->data.AST_LT.left = left;
            node->data.AST_LT.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_LessEq: {
            node->tag = AST_LTE;
            node->data.AST_LTE.left = left;
            node->data.AST_LTE.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_And: {
            node->tag = AST_AND;
            node->data.AST_AND.left = left;
            node->data.AST_AND.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        case Token_Or: {
            node->tag = AST_OR;
            node->data.AST_OR.left = left;
            node->data.AST_OR.right = parse_expr(p, 
                                       precedence_lookup[op.type]);
            return node;
        }
        default: token_loc_print(p->curr); assert(0 && "Not Implemented #1");
    }
}

static AST* parse_expr(Parser* p, Precedence prev_prec) {
    AST* lhs = parse_terminal_expr(p);

    Token curr_op = p->curr;
    Precedence curr_prec = precedence_lookup[curr_op.type];

    while (curr_prec != Precedence_Min) {
        if (prev_prec >= curr_prec) {
            break;
        } else {
            parser_advance(p);
            lhs = parse_infix_expr(p, curr_op, lhs);
            curr_op = p->curr;
            curr_prec = precedence_lookup[curr_op.type];
        }
    }

    return lhs;
}

void parser_advance(Parser* p) {
    p->prev = p->curr;
    p->curr = p->next;
    p->next = lexer_next_token(p->lexer);
}

void parser_parse(Parser* p) {
    struct AST_PROGRAM* stmt_list = &p->ast->data.AST_PROGRAM;
    while (p->curr.type != Token_EOF && stmt_list->stmt_count < 15) {
        p->ast->data.AST_PROGRAM.body[stmt_list->stmt_count++] = parse_stmt(p);
         
        if (p->curr.type != Token_Semicolon) {
            ParserErr(p, p->prev, "Expected Semicolon");
        }

        parser_advance(p);
    }
}
