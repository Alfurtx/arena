#ifndef ARENA_H
#define ARENA_H

#ifndef _WIN32
#error "TODO(fonsi): Other arena backends than WIN32 not implemented yet"
#endif

#ifdef _WIN32
#include <windows.h>
#include <memoryapi.h>
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <memoryapi.h>
#endif

#define Kilobyte(size) (1024LL*size)
#define Megabyte(size) (1024LL*Kilobyte(size))
#define Gigabyte(size) (1024LL*Megabyte(size))

#define ARENA_INIT_RESERVE (Gigabyte(16))
#define ARENA_PAGE_DEFAULT (Megabyte(1))

typedef unsigned int uint;
typedef size_t usize;

typedef struct Arena {
    usize used;
    usize commited;
    void* base;
    void* next_page;

    // TODO(fonsi): probably remove this member cause the maximum virtual memory
    // its just a big upperbound, it is not meant to ever be reached, thus making
    // this member useless
    usize total;
} Arena;

typedef struct ArenaScratch {
    Arena* arena;
    usize pos;
} ArenaScratch;

void* memory_reserve(usize size);
void* memory_commit(void* address, usize size);

ArenaScratch arena_get_scratch(Arena* arena);
void       arena_release_scratch(ArenaScratch* local);
Arena      arena_create(void);
void       arena_release(Arena* arena);
void*      arena_push(Arena* arena, usize size);
void       arena_clear(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

void*
memory_reserve(usize size)
{
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

void*
memory_commit(void* address, usize size)
{
    return VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
}

Arena
arena_create(void)
{
    void* tmp = memory_reserve(ARENA_INIT_RESERVE);
    return (Arena) {
        .used = 0,
        .commited = 0,
        .total = ARENA_INIT_RESERVE,
        .base = tmp,
        .next_page = tmp,
    };
}

void
arena_release(Arena* arena) {
    assert(arena);
    VirtualFree(arena->base, 0, MEM_RELEASE);
}

void
arena_clear(Arena* arena) {
    assert(arena);
    arena->used = 0;
}

void*
arena_push(Arena* arena, usize size)
{
    // commit more memory if it is necessary
    if(arena->used + size >= arena->commited) {
        usize next_page = ARENA_PAGE_DEFAULT;
        // i suppose that this is application dependent and can become a problem
        // but i'll leave it for future fonsi
        if(next_page < size) next_page = size;
        memory_commit(arena->next_page, next_page);
        arena->next_page = ((char*) arena->next_page) + next_page;
        arena->commited += next_page;
    }

    void* result = ((char*) arena->base) + arena->used;
    arena->used += size;

    return result;
}

ArenaScratch
arena_get_scratch(Arena* arena)
{
    assert(arena);
    return (ArenaScratch) {
        .arena = arena,
        .pos = arena->used,
    };
}

void
arena_release_scratch(ArenaScratch* local)
{
    assert(local);
    local->arena->used = local->pos;
}

void
arena_print(Arena* arena)
{
    printf("Arena {\n");
    printf("used:      %zu\n", arena->used);
    printf("commited:  %zu\n", arena->commited);
    printf("total:     %zu\n", arena->total);
    printf("base:      %p\n", arena->base);
    printf("next_page: %p\n", arena->next_page);
    printf("}\n");
}

#endif
