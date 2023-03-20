#define ARENA_IMPLEMENTATION
#include "arena.h"

typedef struct Test {
    float x;
    float y;
    char* buffer[10];
} Test;

void
foo(Arena* arena)
{
    ArenaScratch scratch = arena_get_scratch(arena);
    arena_print(arena);
    Test* test = arena_push(scratch.arena, sizeof(Test));
    arena_print(arena);
    for(uint i = 0; i < 10; i++) {
        test->buffer[i] = arena_push(scratch.arena, sizeof(char) * 100);
        arena_print(arena);
    }
    arena_release_scratch(&scratch);
}

int
main(void)
{
    Arena arena = arena_create();
    arena_print(&arena);
    foo(&arena);
    arena_print(&arena);
    arena_release(&arena);
    return 0;
}
