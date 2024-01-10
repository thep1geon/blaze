#ifndef __HASHMAP_H
#define __HASHMAP_H

#include "arena.h"
#include "string.h"

typedef enum {
    TypeNil,
    TypeStr,
    TypeNum,
    TypeBool, 
} VarType;


#define HASH_MAP_LEN 15000

typedef struct hashmap_t {
    VarType data[HASH_MAP_LEN];
    u64 len;
} HashMap;

u64      hash(String key);
HashMap* hashmap_new(Arena* arena);
VarType  hashmap_get(HashMap* map, String key);
void     hashmap_insert(HashMap* map, String key, VarType val);

#endif  //__HASHMAP_H
