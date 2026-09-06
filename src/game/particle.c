#define MAX_I32 2147483647

// generates a random f32 from 0.0 to 1.0
internal f32
rand_f32()
{
    i32 value = GetRandomValue(0, MAX_I32);
    return (f32)value / (f32)MAX_I32;
}

internal f32
rand_f32_range(f32 min, f32 max)
{
    return min + (rand_f32() * (max - min));
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
    DebugLog("backing mem %p", backing_mem);
    emitter.arena = arena_init(backing_mem, cap);
    emitter.particle_pos = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    DebugLog("backing particle_pos %p", emitter.particle_pos);
    emitter.particle_vel = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    DebugLog("backing particle_vel %p", emitter.particle_vel);
    emitter.particle_time = (f32*)arena_allocate(&emitter.arena, sizeof(f32)*emitter.max_particles);
    DebugLog("backing particle_time %p", emitter.particle_time);

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
            DebugLog("can you work please");
            // NOTE: There is a timing issues with the removing in spawning
            if (emitter->current_particles >= emitter->max_particles)
            {
                DebugLog("In use");
                break;
            }
            // TODO: Change pos to spawn depending on the shape rn it is just point
            emitter->particle_pos[emitter->current_particles] = emitter->pos;
            emitter->particle_vel[emitter->current_particles] = (Vector2){rand_f32_range(emitter->init_vel_min.x, emitter->init_vel_max.x), rand_f32_range(emitter->init_vel_min.y, emitter->init_vel_max.y)};
            emitter->particle_time[emitter->current_particles] = 0.0f;
            emitter->current_particles += 1;

            // NOTE: Placed this here in case of the early break
            // As long as 1 particle is spawned then the timer will reset
            emitter->timer = 0;
        }

    }
    emitter->timer += delta;

    for (i32 i=0; i<emitter->current_particles;)
    {
        // Check for dead particles
        if (emitter->particle_time[i] >= 1.0)
        {
            DebugLog("1 dead %d %d", emitter->current_particles, i);
            emitter->particle_pos[i] = emitter->particle_pos[emitter->current_particles-1];
            emitter->particle_vel[i] = emitter->particle_vel[emitter->current_particles-1];
            emitter->particle_time[i] = emitter->particle_time[emitter->current_particles-1];
            emitter->current_particles -= 1;
            DebugLog("2 dead %d %d", emitter->current_particles, i);
            continue;
        }

        // Update alive particles
        emitter->particle_vel[i].x += emitter->accel.x * delta;
        emitter->particle_vel[i].y += emitter->accel.y * delta;

        emitter->particle_pos[i].x += emitter->particle_vel[i].x * delta;
        emitter->particle_pos[i].y += emitter->particle_vel[i].y * delta;

        DebugLog("id %d particle_pos %f %f", i, emitter->particle_pos[i].x, emitter->particle_pos[i].y);
        // Render particles
        DrawCircleV(emitter->particle_pos[i], 4, RED);

        emitter->particle_time[i] += delta / emitter->life_time;
        ++i;
    }
}
