#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define REGION_DEFAULT_CAPACITY (1024 * 8)

typedef unsigned int uint;

typedef struct Arena {
} Arena;

typedef struct ArenaLocal {
    Arena* arena;
} ArenaLocal;

ArenaLocal arena_get_scratch(Arena* arena);
void       arena_release_scratch(ArenaLocal* local);
void       arena_free(Arena* arena);
void*      arena_push(Arena* arena, size_t size);
void       arena_clear(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

void
arena_free(Arena* arena) {
}

void
arena_clear(Arena* arena) {
}

void*
arena_push(Arena* arena, size_t size)
{
}

ArenaLocal
arena_get_scratch(Arena* arena)
{
}

void
arena_release_scratch(ArenaLocal* local)
{
}
