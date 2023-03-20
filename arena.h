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

#define REGION_DEFAULT_CAPACITY (1024 * 8)

typedef unsigned int uint;

typedef struct Region Region;

void* memcommit(void* address, size_t size);
void* memreserve(size_t size);

struct Region {
    Region* next;
    size_t mem_current;
    size_t mem_total;
    void* data;
};

Region region_new(size_t size);
void region_free(Region* region);

typedef struct Arena {
    Region* begin;
    Region* end;
} Arena;

typedef struct ArenaLocal {
    Arena* arena;
    Region* region;
    size_t pos;
} ArenaLocal;

ArenaLocal arena_get_scratch(Arena* arena);
void       arena_release_scratch(ArenaLocal* local);
void       arena_free(Arena* arena);
void*      arena_push(Arena* arena, size_t size);
void       arena_clear(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

void*
memcommit(void* address, size_t size)
{
    void* result = VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
    return result;
}

void*
memreserve(size_t size)
{
    return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

Region*
region_new(size_t size)
{
    Region* result = calloc(sizeof(Region), 1);
    assert(result);
    result->data = calloc(size);
    result->mem_total = size;
    result->mem_current = 0;
    result->next = NULL;
    return result;
}

void
region_free(Region* region)
{
    assert(region);
    assert(region->data);
    free(region->data);
    region->data = NULL;
    free(region);
}

void
arena_free(Arena* arena) {
    Region* region = arena->begin;
    while(region) {
        Region *r = region;
        region = region->next;
        region_free(r);
    }
    arena->begin = NULL;
    arena->end = NULL;
}

void
arena_clear(Arena* arena) {
    Region* r = arena->begin;
    while(r != NULL) {
        r->mem_current = 0;
        r = r->next;
    }
    arena->end = arena->begin;
}

void*
arena_push(Arena* arena, size_t size)
{
    if(!arena->end) {
        assert(!arena->begin);
        size_t cap = REGION_DEFAULT_CAPACITY;
        if(cap < size) cap = size;
        arena->end = region_new(cap);
        arena->begin = end;
    }

    while(arena->end->mem_current + size > arena->end->mem_total && arena->end->next != NULL)
        arena->end = arena->end->next;

    if(arena->end->mem_current + size > arena->end->mem_total) {
        assert(arena->end->next == NULL);
        size_t cap = REGION_DEFAULT_CAPACITY;
        if(cap < size) cap = size;
        arena->end->next = region_new(cap);
        arena->end = arena->end->next;
    }

    void* result = arena->end->data + arena->end->mem_current;
    arena->end->mem_current += size;
    return result;
}

ArenaLocal
arena_get_scratch(Arena* arena)
{
    assert(arena);
    ArenaLocal result;
    result.arena = arena;
    result.region = arena->end;
    result.pos = arena->end->mem_current;
}

static void
recursive_region_release(Region* r)
{
    if(r == NULL) return;
    recursive_region_release(r->next);
    if(r->next == NULL) {
        region_free(r);
        r = NULL;
    }
}

void
arena_release_scratch(ArenaLocal* local)
{
    assert(local);
    local->arena->end = local->region;
    local->arena->end->mem_current = local->pos;
    recursive_region_release(local->arena->end->next);
}
