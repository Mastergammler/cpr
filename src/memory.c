#include "internal.h"

void arena_alloc(Arena* arena, int size)
{
    arena->size = size;
    arena->cursor = 0;
    arena->mem = malloc(size);
    // set to 0 for simplicity instead of sentinel
    memset(arena->mem, 0, size);

    assert(arena->mem);
}

void arena_reset(Arena* arena)
{
    arena->cursor = 0;
}

void* arena_use(Arena* arena, int size)
{
    assert(arena->cursor + size < arena->size);

    void* memStart = arena->mem + arena->cursor;
    arena->cursor += size;

    return memStart;
}
