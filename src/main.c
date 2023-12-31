#include "include/arena.h"
#include "include/lexer.h"
#include "include/string.h"


int main(void) {
    Arena* arena = arena_new();

    Lexer *l = lexer_new(arena, string("var class and or"));

    Token t;
    while ((t = lexer_next_token(l)).type != Token_EOF) {
        token_print(t);
    }

    arena_free(arena);
    return 0;
}
