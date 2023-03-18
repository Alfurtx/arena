#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARENA_DEFAULT_CAPACITY (1024 * 16)

typedef unsigned int uint;

// typedef struct Region Region;
//
// struct Region {
//     Region* next;
//     size_t mem_current;
//     size_t mem_total;
//     void* data;
// };

typedef struct Arena {
    size_t mem_current;
    size_t mem_total;
    void* data;
} Arena;

typedef struct ArenaLocal {
    Arena* arena;
    size_t pos;
} ArenaLocal;

ArenaLocal arena_get_scratch(Arena* arena);
void arena_release_scratch(ArenaLocal* arena);

// create or destroy an "arena"
// NOTE(fonsi): if not sure about what max_size to input, try with ARENA_DEFAULT_CAPACITY and work from there
Arena arena_alloc(size_t max_size);
void  arena_release(Arena* arena);

// push some bytes into the "arena"
// if size + current arena mem > arena mem total, it will realloc with double the current capacity
void* arena_push(Arena* arena, size_t size);

// some helpers
void arena_clear(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

Arena
arena_alloc(size_t max_size)
{
    return (Arena) {
        .mem_current = 0,
        .mem_total = max_size,
        .data = calloc(max_size, 1),
    };
}

void
arena_release(Arena* arena) {
    assert(arena);
    free(arena->data);
}

void
arena_clear(Arena* arena) {
    memset(arena->data, 0, arena->mem_total);
    arena->mem_current = 0;
}

// TODO(fonsi): rethink the growing/shrinking strategy because if reallocated all previous arena_push given to the code will be pointing to random memory
void*
arena_push(Arena* arena, size_t size)
{
    if(arena->mem_current + size >= arena->mem_total)
        arena->data = realloc(arena->data, arena->mem_total * 2 + size);
    void* result = arena->data + arena->mem_current;
    arena->mem_current += size;
    return result;
}

ArenaLocal
arena_get_scratch(Arena* arena)
{
    assert(arena);
    return (ArenaLocal) {
        .arena = arena,
        .pos = arena->mem_current,
    };
}

void
arena_release_scratch(ArenaLocal* arena)
{
    assert(arena);
    arena.arena.mem_current = arena.pos;
}
