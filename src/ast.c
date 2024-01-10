#include "include/ast.h"
#include "include/hashmap.h"
#include "include/string.h"
#include <assert.h>
#include <stdio.h>

#define emitf fprintf

String vartype_str(VarType type) {
    switch (type) {
        case TypeNil: return string("Nil");
        case TypeNum: return string("Num");
        case TypeStr: return string("Str");
        case TypeBool: return string("Bool");
        default: assert(0 && "Unreachable");
    }
}

AST* ast_new(Arena* a, AST ast) {
    AST* ptr = (AST*)arena_alloc(a, sizeof(AST));
    if (ptr) *ptr = ast;
    return ptr;
}

void ast_print(AST* ast, HashMap* map) {
    if (!ast) {
        return;
    }

    switch (ast->tag) {
        case AST_PROGRAM: {
            struct AST_PROGRAM data = ast->data.AST_PROGRAM;
            printf("Program: \n");
            printf("addr_0:\n");
            for (i32 i = 0; i < (i32)data.stmt_count; ++i) {
                ast_print(data.body[i], map);
                printf("\n");
                printf("addr_%d:\n", i+1);
            }
            printf("-*- End of Program -*-\n");
            return;
        }
        case AST_NIL: {
            printf("nil");
            return;
        }
        case AST_NUMBER: {
            struct AST_NUMBER data = ast->data.AST_NUMBER;
            printf("%.2f",data.val);
            return;
        }
        case AST_STR: {
            struct AST_STR data = ast->data.AST_STR;
            printf("\"%s\"", data.str.data);
            return;
        }
        case AST_IDENT: {
            struct AST_IDENT data = ast->data.AST_IDENT;
            char* type = vartype_str(hashmap_get(map, data.ident)).data;
            printf("(%s) %s", type, data.ident.data);
            return;
        }
        case AST_BOOL: {
            struct AST_BOOL data = ast->data.AST_BOOL;
            printf("%s", data.val ? "true" : "false");
            return;
        }
        case AST_NOT: {
            struct AST_NOT data = ast->data.AST_NOT;
            printf("!(");
            ast_print(data.expr, map);
            printf(")");
            return;
        }
        case AST_AND: {
            struct AST_AND data = ast->data.AST_AND;
            printf("(");
            ast_print(data.left, map);
            printf(" and ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_OR: {
            struct AST_OR data = ast->data.AST_OR;
            printf("(");
            ast_print(data.left, map);
            printf(" or ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_EQ: {
            struct AST_EQ data = ast->data.AST_EQ;
            printf("(");
            ast_print(data.left, map);
            printf(" == ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_NEQ: {
            struct AST_NEQ data = ast->data.AST_NEQ;
            printf("(");
            ast_print(data.left, map);
            printf(" != ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_GT: {
            struct AST_GT data = ast->data.AST_GT;
            printf("(");
            ast_print(data.left, map);
            printf(" > ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_GTE: {
            struct AST_GTE data = ast->data.AST_GTE;
            printf("(");
            ast_print(data.left, map);
            printf(" >= ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_LT: {
            struct AST_LT data = ast->data.AST_LT;
            printf("(");
            ast_print(data.left, map);
            printf(" < ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_LTE: {
            struct AST_LTE data = ast->data.AST_LTE;
            printf("(");
            ast_print(data.left, map);
            printf(" >= ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_ADD: {
            struct AST_ADD data = ast->data.AST_ADD;
            printf("(");
            ast_print(data.left, map);
            printf(" + ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_ADDEQ: {
            struct AST_ADDEQ data = ast->data.AST_ADDEQ;
            printf("%s += ", data.ident.data);
            ast_print(data.expr, map);
            return;
        }
        case AST_SUB: {
            struct AST_SUB data = ast->data.AST_SUB;
            printf("(");
            ast_print(data.left, map);
            printf(" - ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_SUBEQ: {
            struct AST_SUBEQ data = ast->data.AST_SUBEQ;
            printf("%s -= ", data.ident.data);
            ast_print(data.expr, map);
            return;
        }
        case AST_MUL: {
            struct AST_MUL data = ast->data.AST_MUL;
            printf("(");
            ast_print(data.left, map);
            printf(" * ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_MULEQ: {
            struct AST_MULEQ data = ast->data.AST_MULEQ;
            printf("%s *= ", data.ident.data);
            ast_print(data.expr, map);
            return;
        }
        case AST_DIV: {
            struct AST_DIV data = ast->data.AST_DIV;
            printf("(");
            ast_print(data.left, map);
            printf(" / ");
            ast_print(data.right, map);
            printf(")");
            return;
        }
        case AST_DIVEQ: {
            struct AST_DIVEQ data = ast->data.AST_DIVEQ;
            printf("%s /= ", data.ident.data);
            ast_print(data.expr, map);
            return;
        }
        case AST_NEGATE: {
            struct AST_NEGATE data = ast->data.AST_NEGATE;
            printf("-");
            ast_print(data.expr, map);
            return;
        }
        case AST_LET: {
            struct AST_LET data = ast->data.AST_LET;
            char* type = vartype_str(hashmap_get(map, data.ident)).data;
            printf("let %s: %s = ", data.ident.data, type);
            ast_print(data.expr, map);
            return;
        }
        case AST_PRINT: {
            struct AST_PRINT data = ast->data.AST_PRINT;
            printf("print(");
            ast_print(data.expr, map);
            printf(")");
            return;
        }
    }
}

void ast_emit(AST* ast, HashMap* map, FILE* f) {
    if (!ast) {
        return;
    }

    switch (ast->tag) {
        case AST_PROGRAM: {
            struct AST_PROGRAM data = ast->data.AST_PROGRAM;
            emitf(f, "import \"stdlib.mv\"\n\n");
            emitf(f, "jmp init_stdlib\n\n");
            emitf(f, "start__:\n");

            emitf(f, "addr_0:\n");
            for (i32 i = 0; i < (i32)data.stmt_count; ++i) {
                ast_emit(data.body[i], map, f);
                emitf(f, "\n");
                emitf(f, "addr_%d:\n" ,i+1);
            }
            emitf(f, "stop\n");
            return;
        }
        case AST_NIL: {
            emitf(f, "push 0\n");
            return;
        }
        case AST_NUMBER: {
            struct AST_NUMBER data = ast->data.AST_NUMBER;
            emitf(f, "push %.2f\n", data.val);
            return;
        }
        case AST_STR: {
            struct AST_STR data = ast->data.AST_STR;
            emitf(f, "str \"%s\"\n", data.str.data);
            return;
        }
        case AST_IDENT: {
            struct AST_IDENT data = ast->data.AST_IDENT;
            emitf(f, "push %s\n", data.ident.data);
            return;
        }
        case AST_BOOL: {
            struct AST_BOOL data = ast->data.AST_BOOL;
            emitf(f, "push %d\n", data.val);
            return;
        }
        case AST_NOT: {
            struct AST_NOT data = ast->data.AST_NOT;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp\n");
            emitf(f, "call not tmp\n");
            emitf(f, "del tmp\n");
            return;
        }
        case AST_AND: {
            struct AST_AND data = ast->data.AST_AND;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call and a b\n");
            return;
        }
        case AST_OR: {
            struct AST_OR data = ast->data.AST_OR;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call or a b\n");
            return;
        }
        case AST_EQ: {
            struct AST_EQ data = ast->data.AST_EQ;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call eq a b\n");
            return;
        }
        case AST_NEQ: {
            struct AST_NEQ data = ast->data.AST_NEQ;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call neq a b\n");
            return;
        }
        case AST_GT: {
            struct AST_GT data = ast->data.AST_GT;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call gt a b\n");
            return;
        }
        case AST_GTE: {
            struct AST_GTE data = ast->data.AST_GTE;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call gte a b\n");
            return;
        }
        case AST_LT: {
            struct AST_LT data = ast->data.AST_LT;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call lt a b\n");
            return;
        }
        case AST_LTE: {
            struct AST_LTE data = ast->data.AST_LTE;
            ast_emit(data.left, map, f);
            emitf(f, "pop a\n");
            ast_emit(data.right, map, f);
            emitf(f, "pop b\n");
            emitf(f, "call lte a b\n");
            return;
        }
        case AST_ADD: {
            struct AST_ADD data = ast->data.AST_ADD;
            ast_emit(data.left, map, f);
            ast_emit(data.right, map, f);
            emitf(f, "add\n");
            return;
        }
        case AST_ADDEQ: {
            struct AST_ADDEQ data = ast->data.AST_ADDEQ;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp\n");
            emitf(f, "call Add %s tmp | %s\n", data.ident.data, data.ident.data);
            return;
        }
        case AST_SUB: {
            struct AST_SUB data = ast->data.AST_SUB;
            ast_emit(data.left, map, f);
            ast_emit(data.right, map, f);
            emitf(f, "swap\n");
            emitf(f, "sub\n");
            return;
        }
        case AST_SUBEQ: {
            struct AST_SUBEQ data = ast->data.AST_SUBEQ;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp\n");
            emitf(f, "call Sub %s tmp | %s\n", data.ident.data, data.ident.data);
            emitf(f, "push %s\n", data.ident.data);
            return;
        }
        case AST_MUL: {
            struct AST_MUL data = ast->data.AST_MUL;
            ast_emit(data.left, map, f);
            ast_emit(data.right, map, f);
            emitf(f, "mult\n");
            return;
        }
        case AST_MULEQ: {
            struct AST_MULEQ data = ast->data.AST_MULEQ;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp\n");
            emitf(f, "call Mult %s tmp | %s\n", data.ident.data, data.ident.data);
            emitf(f, "push %s\n", data.ident.data);
            return;
        }
        case AST_DIV: {
            struct AST_DIV data = ast->data.AST_DIV;
            ast_emit(data.left, map, f);
            ast_emit(data.right, map, f);
            emitf(f, "swap\n");
            emitf(f, "div\n");
            return;
        }
        case AST_DIVEQ: {
            struct AST_DIVEQ data = ast->data.AST_DIVEQ;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp\n");
            emitf(f, "call Div %s tmp | %s\n", data.ident.data, data.ident.data);
            return;
        }
        case AST_NEGATE: {
            struct AST_NEGATE data = ast->data.AST_NEGATE;
            ast_emit(data.expr, map, f);
            emitf(f, "push -1\n");
            emitf(f, "mult\n");
            return;
        }
        case AST_LET: {
            struct AST_LET data = ast->data.AST_LET;
            ast_emit(data.expr, map, f);
            emitf(f, "pop %s\n", data.ident.data);
            return;
        }
        case AST_PRINT: {
            struct AST_PRINT data = ast->data.AST_PRINT;
            ast_emit(data.expr, map, f);
            emitf(f, "pop tmp_var\n");

            VarType type;
            if (data.expr->tag == AST_IDENT) {
                type = hashmap_get(map, data.expr->data.AST_IDENT.ident);
                if (type == TypeStr) {
                    emitf(f, "call print_str tmp_var\n");
                    goto del;
                } 
            } 
            else if (data.expr->tag == AST_STR) {
                emitf(f, "call print_str tmp_var\n");
                goto del;
            }

            emitf(f, "print tmp_var\n");

        del:
            emitf(f, "del tmp_var\n");
            return;
        }
    }
}
