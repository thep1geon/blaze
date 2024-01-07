#include "include/arena.h"
#include "include/ast.h"
#include "include/err.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/string.h"
#include <stdio.h>

int main(int argc, char** argv) {
    Arena* arena = arena_new();

    if (argc < 2) {
        arena_free(arena);
        puts("Usage: mc [program].m");
        return 5;
    }

    String filepath = string(argv[1]);

    FILE* f = fopen(filepath.data, "rb");
    if (!f) {
        arena_free(arena);
        err("Failed to open file", 0, 0);
    }

    fseek(f, 0L, SEEK_END);
    usize sz = ftell(f);
    rewind(f);

    String src = string_alloc(arena, sz+1);

    fread(src.data, 1, sz, f);

    fclose(f);

    Lexer* lexer = lexer_new(arena, src);

    Parser* parser = parser_new(lexer);

    parser_parse(parser);

    String output_file = string_substring(arena, 
                                          string(argv[1]), 
                                          0, 
                                          string(argv[1]).len - 1);

    output_file = string_concat(arena, output_file, string("mv"));

    if (argc > 3) {
        output_file = string(argv[2]);
    }

    f = fopen(output_file.data, "w");

    // ast_print(parser->ast);
    // printf("\n");

    ast_emit(parser->ast, f);
    // fprintf(f, "print x\n");

    fclose(f);

    arena_free(arena);
    return 0;
}
