#include "include/lexer.h"
#include "include/arena.h"
#include "include/string.h"
#include "include/err.h"

#include <stdio.h>
#include <ctype.h>

static u32 err_line = 0;
static u32 err_col = 0;

#define LexerErr(msg, lexer) do {\
    err_line = lexer->line_number;\
    err_col = lexer->column;\
    lexer_free(lexer);\
    err(msg, err_line, err_col);\
} while (0)

static TokenType token_get_type(String s);
static Token     token_make_string(Lexer* lexer);
static Token     token_make_number(Lexer* lexer);
static Token     token_make_ident(Lexer* lexer);

static void lexer_free(Lexer* lexer);
static void lexer_advance(Lexer* lexer);
static char lexer_peek_offset(Lexer* lexer, usize offset);
static char lexer_peek(Lexer* lexer);
static char lexer_consume(Lexer* lexer);
static bool lexer_bound(Lexer* lexer);
static bool lexer_match(Lexer* lexer, char expected);

Token token_new(TokenType type, String lexeme, u32 line, u32 col) {
    Token t;
    t.type = type;
    t.lexeme = lexeme;
    t.line = line;
    t.col = col;
    return t;
}

String token_type_str(TokenType type) {
    switch (type) {
        case Token_None:        return string("Token_None");
        case Token_Ident:       return string("Token_Ident");
        case Token_Number:      return string("Token_Number");
        case Token_String:      return string("Token_String");
        case Token_Plus:        return string("Token_Plus");
        case Token_Dash:        return string("Token_Dash");
        case Token_Star:        return string("Token_Star");
        case Token_Slash:       return string("Token_Slash");
        case Token_Comma:       return string("Token_Comma");
        case Token_Dot:         return string("Token_Dot");
        case Token_Semicolon:   return string("Token_Semicolon");
        case Token_Greater:     return string("Token_Greater");
        case Token_Less:        return string("Token_Less");
        case Token_Eq:          return string("Token_Eq");
        case Token_LParen:      return string("Token_LParen");
        case Token_RParen:      return string("Token_RParen");
        case Token_RCurly:      return string("Token_RCurly");
        case Token_LCurly:      return string("Token_LCurly");
        case Token_LBrace:      return string("Token_LBrace");
        case Token_RBrace:      return string("Token_RBrace");
        case Token_DoubleEq:    return string("Token_DoubleEq");
        case Token_DoubleColon: return string("Token_DoubleColon");
        case Token_LessEq:      return string("Token_LessEq");
        case Token_GreaterEq:   return string("Token_GreaterEq");
        case Token_Inc:         return string("Token_Inc");
        case Token_Dec:         return string("Token_Dec");
        case Token_PlusEq:      return string("Token_PlusEq");
        case Token_MinusEq:     return string("Token_PlusEq");
        case Token_MultEq:      return string("Token_PlusEq");
        case Token_DivEq:       return string("Token_PlusEq");
        case Token_Range:       return string("Token_Range");
        case Token_If:          return string("Token_If");
        case Token_Else:        return string("Token_Else");
        case Token_Elif:        return string("Token_Elif");
        case Token_For:         return string("Token_For");
        case Token_Bang:        return string("Token_Bang");
        case Token_While:       return string("Token_While");
        case Token_Colon:       return string("Token_Colon");
        case Token_At:          return string("Token_At");
        case Token_Func:        return string("Token_Func");
        case Token_Class:       return string("Token_Class");
        case Token_Let:         return string("Token_Let");
        case Token_True:        return string("Token_True");
        case Token_False:       return string("Token_False");
        case Token_And:         return string("Token_And");
        case Token_Or:          return string("Token_Or");
        case Token_Return:      return string("Token_Return");
        case Token_Nil:         return string("Token_Nil");
        case Token_NotEq:         return string("Token_Not");
        case Token_EOF:         return string("Token_EOF");
        case Token_Unexpected:  return string("Token_Unexpected");
        case TokenTypeCount:    return string("TokenTypeCount");
        default:                return string("Unreachable");
    }
}

void token_print(Token t) {
    printf("Token [Type: %s, Lexeme: %s]\n", token_type_str(t.type).data, t.lexeme.data);
}

void token_loc_print(Token t) {
    printf("Token [Type: %s, Lexeme: %s] %d:%d\n", 
           token_type_str(t.type).data, 
           t.lexeme.data,
           t.line,
           t.col);
}

static TokenType token_get_type(String s) {
    if (string_eq(string("and"), s)) {
        return Token_And;
    }
    else if (string_eq(string("class"), s)) {
        return Token_Class;
    }
    else if (string_eq(string("elif"), s)) {
        return Token_Elif;
    }
    else if (string_eq(string("else"), s)) {
        return Token_Else;
    }
    else if (string_eq(string("false"), s)) {
        return Token_False;
    }
    else if (string_eq(string("for"), s)) {
        return Token_For;
    }
    else if (string_eq(string("Func"), s)) {
        return Token_Func;
    }
    else if (string_eq(string("if"), s)) {
        return Token_If;
    }
    else if (string_eq(string("let"), s)) {
        return Token_Let;
    }
    else if (string_eq(string("nil"), s)) {
        return Token_Nil;
    }
    else if (string_eq(string("not"), s)) {
        return Token_NotEq;
    }
    else if (string_eq(string("or"), s)) {
        return Token_Or;
    }
    else if (string_eq(string("return"), s)) {
        return Token_Return;
    }
    else if (string_eq(string("true"), s)) {
        return Token_True;
    }
    else if (string_eq(string("while"), s)) {
        return Token_While;
    }

    return Token_Ident;
}

static Token token_make_string(Lexer* lexer) {
    usize buf_len = 0;
    usize src_ptr = lexer->cursor;
    char c;
    while ((c = lexer->src.data[src_ptr++]) && c != '"') {
        if (c == '\\') {
            c = lexer->src.data[src_ptr];

            switch (c) {
                case 'n': buf_len++; src_ptr++; break;
                case 'r': buf_len++; src_ptr++; break;
                case 't': buf_len++; src_ptr++; break;
                case 'b': buf_len++; src_ptr++; break;
                case '\'': buf_len++; src_ptr++; break;
                case '"': buf_len++; src_ptr++; break;
                case '\\': buf_len++; src_ptr++; break;
                default: break;
            }
        }
        buf_len++;
    }

    String buf = string_alloc(lexer->arena, buf_len+1);
    u64 buf_ptr = 0;
    while (!lexer_bound(lexer) && lexer_peek(lexer) != '"') {
        char c = lexer_peek(lexer);
        if (c == '\\') {
            lexer_advance(lexer);

            switch (lexer_consume(lexer)) {
                case 'n': buf.data[buf_ptr++] = '\n'; break;
                case 't': buf.data[buf_ptr++] = '\t'; break;
                case 'r': buf.data[buf_ptr++] = '\r'; break;
                case 'b': buf.data[buf_ptr++] = '\b'; break;
                case '\'': buf.data[buf_ptr++] = '\''; break;
                case '"': buf.data[buf_ptr++] = '"'; break;
                case '\\': buf.data[buf_ptr++] = '\\'; break;
                default: 
                    LexerErr("Unknown Escape Char", lexer);

            }
        } else {
            buf.data[buf_ptr++] = c;
            lexer_advance(lexer);
        }
    }

    if (lexer_bound(lexer)) {
        LexerErr("Unterminated String", lexer);
    } 

    lexer_advance(lexer);

    return token_new(Token_String, buf, lexer->line_number, lexer->column);
}

static Token token_make_number(Lexer* lexer) {
    usize buf_len = 0;
    usize src_ptr = lexer->cursor;
    char c;
    while ((c = lexer->src.data[src_ptr++]) && 
        (isdigit(c) || c == '.')) {
        buf_len++;
    }

    String buf = string_alloc(lexer->arena, buf_len+1);
    u64 buf_ptr = 0;
    bool dec_point = false;

    buf.data[buf_ptr++] = lexer_peek_offset(lexer, -1);

    while (!lexer_bound(lexer) &&
    (isdigit(lexer_peek(lexer)) || lexer_peek(lexer) == '.')) {
        char c = lexer_peek(lexer);
        if (c == '.') {
            if (lexer_peek_offset(lexer, 1) == '.') {
                return token_new(Token_Number, buf, lexer->line_number, lexer->column);
            }

            if (dec_point) {
                LexerErr("Multiple Decimals in Float", lexer);
            } 

            dec_point = true; 
        } 

        buf.data[buf_ptr++] = c;
        lexer_advance(lexer);
    }

    return token_new(Token_Number, buf, lexer->line_number, lexer->column);
}

static Token token_make_ident(Lexer* lexer) {
    usize buf_len = 0;
    usize src_ptr = lexer->cursor;
    char c;
    while ((c = lexer->src.data[src_ptr++]) && 
        (isalnum(c) || c == '_')) {
        buf_len++;
    }

    String buf = string_alloc(lexer->arena, buf_len+1);
    u64 buf_ptr = 0;

    buf.data[buf_ptr++] = lexer_peek_offset(lexer, -1);

    while (!lexer_bound(lexer) &&
            (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_')) {
          buf.data[buf_ptr++] = lexer_consume(lexer);
    }

    buf.len = buf_ptr;

    // return token_new(token_get_type(buf), buf);

    TokenType type = token_get_type(buf);
    if (type == Token_Ident) {
        return token_new(type, buf, lexer->line_number, lexer->column);
    } else {
        arena_dealloc_last(lexer->arena);
        return token(type, lexer->line_number, lexer->column);
    }
}

/*
*  Lexer Code Begins Here
*/

Lexer* lexer_new(Arena* a, String src) {
    Lexer* l = (Lexer*)arena_alloc(a, sizeof(Lexer));

    l->src = src;
    l->cursor = 0;
    l->column = 0;
    l->line_number = 1;

    l->arena = a;
    
    return l;
}

static void lexer_free(Lexer* lexer) {
    arena_free(lexer->arena);
}

static bool lexer_bound(Lexer* lexer) {
    return lexer->src.data[lexer->cursor] == '\0';
}

static char lexer_peek_offset(Lexer* lexer, usize offset) {
    return lexer->src.data[lexer->cursor+offset];
}

static char lexer_peek(Lexer* lexer) {
    return lexer_peek_offset(lexer, 0);
}

static bool lexer_match(Lexer* lexer, char expected) {
    if (lexer_bound(lexer)) return false;
    if (lexer_peek(lexer) != expected) return false;

    lexer_advance(lexer);
    return true;
}

static char lexer_consume(Lexer* lexer) {
    char c = lexer_peek(lexer);
    lexer_advance(lexer);
    return c;
}

static void lexer_advance(Lexer* lexer) {
    lexer->cursor++;
    lexer->column++;
}

Token lexer_next_token(Lexer* lexer) {
    if (lexer_bound(lexer)) {
        return token(Token_EOF, lexer->line_number, lexer->column);
    }

    char c = lexer_consume(lexer);

    if (c == '\n') {
        lexer->line_number++;
        lexer->column = 0;
    }

    if (isspace(c)) {
        return lexer_next_token(lexer);
    }
    switch (c) {
        case '=': {
            if (lexer_match(lexer, '=')) {
                return token(Token_DoubleEq, lexer->line_number, lexer->column);
            }

            return token(Token_Eq, lexer->line_number, lexer->column);
        }
        case '>': {
            if (lexer_match(lexer, '=')) {
                return token(Token_GreaterEq, lexer->line_number, lexer->column);
            } 

            return token(Token_Greater, lexer->line_number, lexer->column);
        }
        case '<': {
            if (lexer_match(lexer, '=')) {
                return token(Token_LessEq, lexer->line_number, lexer->column);
            } 

            return token(Token_Less, lexer->line_number, lexer->column);
        }
        case ':': {
            if (lexer_match(lexer, ':')) {
                return token(Token_DoubleColon, lexer->line_number, lexer->column);
            } 

            return token(Token_Colon, lexer->line_number, lexer->column);
        }
        case '.': {
            if (lexer_match(lexer, '.')) {
                return token(Token_Range, lexer->line_number, lexer->column);
            }
            return token(Token_Dot, lexer->line_number, lexer->column);
        }
        case '+': {
            if (lexer_match(lexer, '+')) {
                return token(Token_Inc, lexer->line_number, lexer->column);
            } 
            else if (lexer_match(lexer, '=')) {
                return token(Token_PlusEq, lexer->line_number, lexer->column);
            }
            return token(Token_Plus, lexer->line_number, lexer->column);
        }
        case '-': {
            if (lexer_match(lexer, '-')) {
                return token(Token_Dec, lexer->line_number, lexer->column);
            } 
            else if (lexer_match(lexer, '=')) {
                return token(Token_MinusEq, lexer->line_number, lexer->column);
            }
            return token(Token_Dash, lexer->line_number, lexer->column);
        }
        case '*': {
            if (lexer_match(lexer, '=')) {
                return token(Token_MultEq, lexer->line_number, lexer->column);
            } 

            return token(Token_Star, lexer->line_number, lexer->column);
        }
        case '/': {
            if (lexer_match(lexer, '=')) {
                return token(Token_DivEq, lexer->line_number, lexer->column);
            } 

            return token(Token_Slash, lexer->line_number, lexer->column);
        }
        case '!': return token(Token_Bang, lexer->line_number, lexer->column);
        case ',': return token(Token_Comma, lexer->line_number, lexer->column);
        case ';': return token(Token_Semicolon, lexer->line_number, lexer->column);
        case '\'': case '"': return token_make_string(lexer);
        case '{': return token(Token_LCurly, lexer->line_number, lexer->column);
        case '}': return token(Token_RCurly, lexer->line_number, lexer->column);
        case '[': return token(Token_LBrace, lexer->line_number, lexer->column);
        case ']': return token(Token_RBrace, lexer->line_number, lexer->column);
        case '(': return token(Token_LParen, lexer->line_number, lexer->column);
        case ')': return token(Token_RParen, lexer->line_number, lexer->column);
        case '@': return token(Token_At, lexer->line_number, lexer->column);
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            return token_make_number(lexer);
        }
        default: {
            if (isalpha(c)) {
                return token_make_ident(lexer);
            }

            LexerErr("Unexpected Token", lexer);
        }
    }

    return token(Token_Unexpected, lexer->line_number, lexer->column);
}
