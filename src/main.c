#include "include/arena.h"
#include "include/defines.h"
#include "include/ast.h"
#include "include/err.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/string.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    bool run = false;
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
    
    String src = string_alloc(arena, sz);

    
    fread(src.data, sizeof(char), src.len, f);
    
    fclose(f);
    
    Lexer* lexer = lexer_new(arena, src);
    
    Parser* parser = parser_new(lexer);
    
    parser_parse(parser);
    
    String output_file = string_substring(arena, 
                                          string(argv[1]), 
                                          0, 
                                          string(argv[1]).len);
    
    output_file = string_concat(arena, output_file, string("mv"));
    
    if (argc >= 3) {
        if (string_eq(string(argv[2]), string("db"))) {
            ast_print(parser->ast, parser->var_map);
            printf("\n");
        } 
        else if (string_eq(string(argv[2]), string("-r"))) {
            run = true;
        }
        else {
            output_file = string(argv[2]);
        }
    }

    f = fopen(output_file.data, "w");
    
    ast_emit(parser->ast, parser->var_map, f);
    
    fclose(f);
    
    if (run) {
        String cmd = string_concat(arena, string("mvi "), output_file);
        system(cmd.data);
    }

    arena_free(arena);
    return 0;
}
