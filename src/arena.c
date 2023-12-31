#include "include/arena.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// TODO: Change all of the asserts to actually handle potential error

Arena* arena_new() {
    u64 size = ARENA_DEFAULT_SIZE;
    Arena* a = malloc(sizeof(Arena));
    a->mem = (void*) malloc(size);
    memset(a->mem, 0, size);
    a->mem_len = size;
    a->pos = a->mem;
    a->pos_u64 = 0;
    return a;
}

void arena_free(Arena *a) {
    free(a->mem);
    free(a);
}

static void arena_resize(Arena* a) {
    a->mem_len *= 2;
    a->mem = realloc(a->mem, a->mem_len);
    memset((void*)((char*)a->mem+a->pos_u64), 0, a->mem_len-a->pos_u64);
}

// void* arena_resize(Arena* a, )

void* arena_alloc(Arena* a, u64 size) {
    if (a->mem_len - a->pos_u64 < size) {
        arena_resize(a);
    }

    void* ret_mem = a->pos;

    a->pos = (void*)((char*)a->pos+size);
    a->last_pos_u64 = a->pos_u64;
    a->pos_u64 += size;

    return ret_mem;
}

void* arena_alloc_zero(Arena* a, u64 size) {
    void* mem = arena_alloc(a, size);  
    memset(mem, 0, size);
    return mem;
}

void arena_dealloc(Arena* a, u64 size) {
    assert((i64)(a->pos_u64 - size) >= 0);
    a->pos_u64 -= size; 

    a->pos = (void*)((char*)a->pos - size);
    memset(a->pos, 0, a->mem_len-a->pos_u64);
}

void arena_dealloc_last(Arena *a) {
    arena_dealloc(a, a->pos_u64-a->last_pos_u64);  
}

void arena_set_pos_back(Arena* a, u64 pos) {
    assert(pos < a->pos_u64); 
    arena_dealloc(a, a->pos_u64 - pos); 
}

void arena_clear(Arena* a) {
    memset(a->mem, 0, a->mem_len);
    arena_set_pos_back(a, 0);
}

void arena_dump_mem(Arena* a) {
    for (u64 i = 0; i < a->mem_len; ++i) {
        if (i == a->pos_u64) printf("^");
        printf("%02hhx ", ((char*)a->mem)[i]);


        if ((i+1) % 16 == 0) {
            printf("\n");
        }
    }
}
