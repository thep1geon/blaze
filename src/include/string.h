#ifndef __STRING_H
#define __STRING_H

#include "arena.h"
#include "defines.h"

typedef struct string_t {
    char* data;
    u64 len;
} String;

String string(char* string_lit);
String string_format(Arena* a, const char* format, ...);
String string_alloc(Arena *a, u64 len);
String string_alloc_str(Arena* a, char* lit);

void   string_print(String s);
const  char* string_cstr(Arena* a, String str);

String string_substring(Arena* a, String str, u64 start, u64 end);
String string_concat(Arena* a, String str_a, String str_b);
String string_replace(Arena* a, String str, String needle, String replacement);

u64    string_index_of(String str, String substr);
f64    string_to_number(String str);
bool   string_eq(String a, String b);

#endif  //__STRING_H
