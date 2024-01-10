#include "include/string.h"
#include "include/arena.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

String string(char* string_lit) {
    String s;
    s.data = string_lit;
    s.len = strlen(s.data);
    return s;
}

String string_format(Arena* a, const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the size of the formatted string
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, format, args_copy) + 1; // +1 for the null terminator
    va_end(args_copy);

    // Allocate a buffer to hold the formatted string
    char* buffer = (char*)arena_alloc(a, size);

    // Format the string
    vsnprintf(buffer, size, format, args);

    va_end(args);

    String result;
    result.data = buffer;
    result.len = size - 1; // Exclude the null terminator from the length

    return result;
}

String string_alloc_str(Arena* a, char* lit) {
    String s = string_alloc(a, strlen(lit));
    memcpy(s.data, lit, s.len);
    return s;
}

String string_alloc(Arena* a, u64 len) {
    String s;
    s.data = (char*)arena_alloc(a, len+1);
    s.len = len;
    return s;
}

String string_substring(Arena* a, String str, u64 start, u64 end) {
    assert(start < end);
    String s = string_alloc(a, end-start);
    s.len--;

    char* ptr = s.data;
    for (u64 i = start; i < end; ++i) {
        *ptr++ = str.data[i];
    }


    return s;
}

const char* string_cstr(Arena* a, String str) {
    char* ptr = (char*)arena_alloc(a, str.len); 

    for (u64 i = 0; i < str.len; ++i) {
        ptr[i] = str.data[i];
    }

    return ptr;
}

void string_print(String s) {
    for (u64 i = 0; i < s.len; ++i) {
        putchar(s.data[i]);
    }

    putchar('\n');
}

u64 string_index_of(String str, String substr) {
    if (substr.len > str.len) {
        return -1;
    }

    bool found = false;
    for (u64 index = 0; index < str.len; ++index) {
        u64 substr_ptr = 0;
        if (str.data[index] == substr.data[substr_ptr++]) {
            found = true;

            for (u64 i = index+1; substr_ptr < substr.len-1; ++i) {
                if (str.data[i] != substr.data[substr_ptr++]) {
                    found = false;
                    break;
                }
            }

            if (found) {
                return index;
            }
        }
    }

    return -1;
}

String string_concat(Arena* a, String str_a, String str_b) {
    String new_str = string_alloc(a, str_a.len + str_b.len);

    u64 a_ptr = 0; 
    u64 b_ptr = 0; 

    for (u64 i = 0; i < new_str.len; ++i) {
        char c = i < str_a.len ? 
                        str_a.data[a_ptr++] :
                        str_b.data[b_ptr++];
        new_str.data[i] = c;
    }

    return new_str;
}

String string_replace(Arena* a, String str, String needle, String replacement) {
    Arena* scratch = arena_new();
    char* cstr = (char*)string_cstr(scratch, str);
    char* needle_cstr = (char*)string_cstr(a, needle);

    size_t count = 0;
    char* pos = cstr;

    while ((pos = strstr(pos, needle_cstr)) != NULL) {
        count++;
        pos += needle.len;
    }

    size_t new_len = str.len + count * (replacement.len - needle.len);

    String s = string_alloc(a, new_len);

    char* dest = s.data;
    pos = cstr;

    while (count > 0) {
        char* start = strstr(pos, needle_cstr);
        size_t segment_len = start - pos;
        memcpy(dest, pos, segment_len);
        dest += segment_len;
        memcpy(dest, replacement.data, replacement.len);
        dest += replacement.len;
        pos = start + needle.len;
        count--;
    }

    strcpy(dest, pos);

    arena_free(scratch);
    return s;
}

f64 string_to_number(String str) {
    return atof(str.data);
}

bool string_eq(String a, String b) {
    if (a.len != b.len) return false;

    for (u64 i = 0; i < a.len; ++i) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }

    return true;
}
