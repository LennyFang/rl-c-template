typedef struct Arena Arena;
struct Arena
{
    void* init_pos;
    u64 len; // In bytes
    u64 cap; // In bytes
};

// Bring your own backing allocator lol
Arena
arena_init(void* init_pos, u64 cap);

// Returns NULL if there is no capacity for the allocation
void*
arena_allocate(Arena* arena, u64 size);

// Doesn't actually free the memory pointer but just sets the length back to 0
void
arena_clear(Arena* arena);
