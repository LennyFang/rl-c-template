// Bring your own backing allocator lol
Arena
arena_init(void* init_pos, u64 cap)
{
    Arena arena = {0};
    arena.cap = cap;
    arena.init_pos = init_pos;
    return arena;
}

// Returns NULL if there is no capacity for the allocation
void*
arena_allocate(Arena* arena, u64 size)
{
    if (arena->len + size > arena->cap) {
        return NULL;
    }
    void* block = (void*)(((u8*)arena->init_pos) + arena->len);
    arena->len += size;
    return block;
}

// Doesn't actually free the memory pointer but just sets the length back to 0
void
arena_clear(Arena* arena)
{
    arena->len = 0;
}
