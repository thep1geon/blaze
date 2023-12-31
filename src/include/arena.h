#ifndef ARENA_H_
#define ARENA_H_

#include "defines.h"

#ifndef ARENA_DEFAULT_SIZE
#define ARENA_DEFAULT_SIZE 1024
#endif

typedef struct {
    void* mem;
    u64   mem_len;
    void* pos;
    u64   pos_u64;
    u64   last_pos_u64;
} Arena;

Arena* arena_new();
void   arena_free(Arena* a);

void* arena_alloc(Arena* a, u64 size);
void* arena_alloc_zero(Arena* a, u64 size);

// some macro helpers that I've found nice:
#define AllocArray(arena, type, count) (type *)arena_alloc((arena), sizeof(type)*(count))
#define AllocArrayZero(arena, type, count) (type *)arena_alloc_zero((arena), sizeof(type)*(count))
#define AllocStruct(arena, type) PushArray((arena), (type), 1)
#define AllocStructZero(arena, type) PushArrayZero((arena), (type), 1)

void arena_dealloc(Arena* a, u64 size);
void arena_dealloc_last(Arena* a);

void arena_set_pos_back(Arena* a, u64 pos);
void arena_clear(Arena* a);

void arena_dump_mem(Arena* a);

#endif // ARENA_H_
