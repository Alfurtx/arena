#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Arena {
    size_t mem_current;
    size_t mem_total;
    void* data;
};

#endif // ARENA_H
