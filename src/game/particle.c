#define MAX_I32 2147483647

internal f32
rand_f32()
{
    i32 value = GetRandomValue(0, MAX_I32);
    return (f32)value / (f32)MAX_I32;
}

Emitter 
init_emitter(Arena* backing_arena, f32 life_time, f32 spawn_rate, u32 spawn_amount)
{
    Emitter emitter = {0};
    emitter.spawn_rate = spawn_rate;
    emitter.spawn_amount = spawn_amount;
    emitter.life_time = life_time;
    emitter.max_particles = (u32)((spawn_amount * life_time) / spawn_rate + 0.99f);

    // Allocations
    u64 cap = (sizeof(Vector2)+sizeof(Vector2)+sizeof(f32))*emitter.max_particles;
    void* backing_mem = arena_allocate(backing_arena, cap);
    printf("backing mem %p\n", backing_mem);
    emitter.arena = arena_init(backing_mem, cap);
    emitter.pos = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    printf("backing pos %p\n", emitter.pos);
    emitter.vel = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    printf("backing vel %p\n", emitter.vel);
    emitter.time = (f32*)arena_allocate(&emitter.arena, sizeof(f32)*emitter.max_particles);
    printf("backing time %p\n", emitter.time);

    return emitter;
}

void 
destory_emitter(Emitter* emitter)
{
}

void uar_emitter(Emitter* emitter, f32 delta)
{
    // Spawning Logic
    if (emitter->timer > emitter->spawn_rate)
    {
        for (i32 i=0; i<emitter->spawn_amount; i++)
        {
            printf("can you work please\n");
            // NOTE: There is a timing issues with the removing in spawning
            if (emitter->current_particles >= emitter->max_particles)
            {
                printf("In use\n");
                break;
            }
            // TODO: Remove the temp variables for pos and vel
            emitter->pos[emitter->current_particles] = (Vector2){100, 100};
            emitter->vel[emitter->current_particles] = (Vector2){100, 100};
            emitter->time[emitter->current_particles] = 0.0f;
            emitter->current_particles += 1;

            // NOTE: Placed this here in case of the early break
            // As long as 1 particle is spawned then the timer will reset
            emitter->timer = 0;
        }

    }
    emitter->timer += delta;

    for (i32 i=0; i<emitter->current_particles;)
    {
        printf("time %f\n", emitter->time[i]);
        // Check for dead particles
        if (emitter->time[i] >= 1.0)
        {
            printf("\n1 dead %d %d\n", emitter->current_particles, i);
            emitter->pos[i] = emitter->pos[emitter->current_particles-1];
            emitter->vel[i] = emitter->vel[emitter->current_particles-1];
            emitter->time[i] = emitter->time[emitter->current_particles-1];
            emitter->current_particles -= 1;
            printf("\n2 dead %d %d\n", emitter->current_particles, i);
            continue;
        }
        // Update alive particles
        emitter->pos[i].x += emitter->vel[i].x * delta;
        emitter->pos[i].y += emitter->vel[i].y * delta;
        emitter->time[i] += delta / emitter->life_time;
        printf("id %d pos %f %f\n", i, emitter->pos[i].x, emitter->pos[i].y);
        // Render particles
        DrawCircleV(emitter->pos[i], 10, RED);
        ++i;
    }
}
