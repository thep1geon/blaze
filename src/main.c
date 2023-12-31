#include "include/arena.h"
#include "include/err.h"
#include "include/lexer.h"
#include "include/string.h"
#include <stdio.h>
#include <sys/stat.h>


int main(void) {
    Arena* arena = arena_new();

    FILE* f = fopen("app.m", "rb");
    if (!f) {
        err("Failed to open file", __LINE__, 0);
    }

    usize sz = 0;
    fseek(f, 0L, SEEK_END);
    sz = ftell(f);
    rewind(f);

    String s = string_alloc(arena, sz);
    usize s_ptr = 0;

    char c;
    while ((c = fgetc(f)) != EOF) {
        s.data[s_ptr++] = c;
    }

    fclose(f);

    Lexer *l = lexer_new(arena, s);

    Token t;
    while ((t = lexer_next_token(l)).type != Token_EOF) {
        token_print(t);
    }

    arena_free(arena);
    return 0;
}
