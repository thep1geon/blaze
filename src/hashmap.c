#include "include/hashmap.h"
#include "include/arena.h"
#include "include/ast.h"

u64 hash(String key) {
    u64 hash = 0; 

    for (u64 i = 0; i < key.len; ++i) {
        // hash = (hash + (u64)*key.data+i) << i;
        hash = (hash + (u64)*key.data+i) << (5 * i);
        hash += key.len * i;
    }

    hash %= HASH_MAP_LEN;

    return hash;
}

HashMap* hashmap_new(Arena* arena) {
    HashMap* map = arena_alloc(arena, sizeof(HashMap));
    map->len = HASH_MAP_LEN;
    return map;
}

VarType hashmap_get(HashMap* map, String key) {
    return map->data[hash(key)];
}

void hashmap_insert(HashMap* map, String key, VarType val) {
    map->data[hash(key)] = val;
}
