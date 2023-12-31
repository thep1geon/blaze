#include "include/arena.h"
#include "include/err.h"
#include "include/lexer.h"
#include "include/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


int main(int argc, char** argv) {
    Arena* arena = arena_new();

    if (argc < 2) {
        arena_free(arena);
        puts("Usage: mc [program].m");
        exit(5);
    }

    String filepath = string(argv[1]);

    FILE* f = fopen(filepath.data, "rb");
    if (!f) {
        arena_free(arena);
        err("Failed to open file", __LINE__, 0);
    }

    fseek(f, 0L, SEEK_END);
    usize sz = ftell(f);
    rewind(f);

    String src = string_alloc(arena, sz);
    usize s_ptr = 0;

    char c;
    while ((c = fgetc(f)) != EOF) {
        src.data[s_ptr++] = c;
    }

    fclose(f);

    Lexer *l = lexer_new(arena, src);

    Token t;
    while ((t = lexer_next_token(l)).type != Token_EOF) {
        token_print(t);
    }

    // arena_dump_mem(arena);
    
    arena_free(arena);
    return 0;
}
