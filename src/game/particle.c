#define MAX_I32 2147483647

// generates a random f32 from 0.0 to 1.0
internal inline f32
rand_f32()
{
    i32 value = GetRandomValue(0, MAX_I32);
    return (f32)value / (f32)MAX_I32;
}

internal inline f32
rand_f32_range(f32 min, f32 max)
{
    return min + (rand_f32() * (max - min));
}

internal inline u8
lerp_u8(u8 init, u8 final, f32 time)
{
    return init + (u8)(((f32)(final-init))*time);
}

internal Color
lerp_colour(Color init, Color final, f32 time)
{
    return (Color){lerp_u8(init.r, final.r, time), lerp_u8(init.g, final.g, time), lerp_u8(init.b, final.b, time), lerp_u8(init.a, final.a, time)};
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
    u64 cap = (sizeof(Vector2)+sizeof(Vector2)+sizeof(f32)+sizeof(f32)+sizeof(f32))*emitter.max_particles;
    void* backing_mem = arena_allocate(backing_arena, cap);
    DebugLog("backing mem %p", backing_mem);
    emitter.arena = arena_init(backing_mem, cap);
    emitter.particle_pos = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    DebugLog("backing particle_pos %p", emitter.particle_pos);
    emitter.particle_vel = (Vector2*)arena_allocate(&emitter.arena, sizeof(Vector2)*emitter.max_particles);
    DebugLog("backing particle_vel %p", emitter.particle_vel);
    emitter.particle_rotation = (f32*)arena_allocate(&emitter.arena, sizeof(f32)*emitter.max_particles);
    DebugLog("backing particle_size %p", emitter.particle_time);
    emitter.particle_size = (f32*)arena_allocate(&emitter.arena, sizeof(f32)*emitter.max_particles);
    DebugLog("backing particle_rotation %p", emitter.particle_time);
    emitter.particle_time = (f32*)arena_allocate(&emitter.arena, sizeof(f32)*emitter.max_particles);
    DebugLog("backing particle_time %p", emitter.particle_time);

    return emitter;
}

void 
destory_emitter(Emitter* emitter)
{
}

void 
uar_emitter(Emitter* emitter, Texture2D tex_atlas, f32 delta)
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

            switch (emitter->shape)
            {
                case Shape_Point:
                    emitter->particle_pos[emitter->current_particles] = emitter->pos;
                    emitter->particle_vel[emitter->current_particles] = (Vector2){rand_f32_range(emitter->init_vel_min.x, emitter->init_vel_max.x), rand_f32_range(emitter->init_vel_min.y, emitter->init_vel_max.y)};
                    break;
                case Shape_Rectangle:
                    emitter->particle_pos[emitter->current_particles] = (Vector2){rand_f32_range(emitter->pos.x, emitter->pos.x + emitter->size.dim.x), rand_f32_range(emitter->pos.y, emitter->pos.y + emitter->size.dim.y)};
                    emitter->particle_vel[emitter->current_particles] = (Vector2){rand_f32_range(emitter->init_vel_min.x, emitter->init_vel_max.x), rand_f32_range(emitter->init_vel_min.y, emitter->init_vel_max.y)};
                    break;
                case Shape_Circle:
                    f32 angle = rand_f32_range(0, PI*2.0);
                    f32 len = rand_f32_range(emitter->size.inner_radius, emitter->size.outer_radius);
                    Vector2 dir_vec = {cosf(angle), sinf(angle)};
                    emitter->particle_pos[emitter->current_particles] = (Vector2){emitter->pos.x + dir_vec.x*len, emitter->pos.y + dir_vec.y*len};
                    if (emitter->normal_vel) 
                    {
                        f32 speed = rand_f32_range(emitter->init_normal_vel_min, emitter->init_normal_vel_max);
                        emitter->particle_vel[emitter->current_particles] = (Vector2){dir_vec.x*speed, dir_vec.y*speed};
                    } 
                    else
                    {
                        emitter->particle_vel[emitter->current_particles] = (Vector2){rand_f32_range(emitter->init_vel_min.x, emitter->init_vel_max.x), rand_f32_range(emitter->init_vel_min.y, emitter->init_vel_max.y)};
                    }
                    break;
            }
            emitter->particle_size[emitter->current_particles] = rand_f32_range(emitter->init_size_min, emitter->init_size_max);
            emitter->particle_rotation[emitter->current_particles] = rand_f32_range(emitter->init_rotation_min, emitter->init_rotation_max);
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
            emitter->particle_size[i] = emitter->particle_size[emitter->current_particles-1];
            emitter->particle_rotation[i] = emitter->particle_rotation[emitter->current_particles-1];
            emitter->particle_time[i] = emitter->particle_time[emitter->current_particles-1];
            emitter->current_particles -= 1;
            DebugLog("2 dead %d %d", emitter->current_particles, i);
            continue;
        }

        // Update alive particles
        emitter->particle_rotation[i] += emitter->rotation_speed * delta;

        emitter->particle_vel[i].x += emitter->accel.x * delta;
        emitter->particle_vel[i].y += emitter->accel.y * delta;

        emitter->particle_pos[i].x += emitter->particle_vel[i].x * delta;
        emitter->particle_pos[i].y += emitter->particle_vel[i].y * delta;

        DebugLog("id %d particle_pos %f %f", i, emitter->particle_pos[i].x, emitter->particle_pos[i].y);
        // Render particles
        // TODO: Change this to use the size param & colour params of the emitter
        // DrawCircleV(emitter->particle_pos[i], 2, lerp_colour(SKYBLUE, ColorAlpha(RED, 0.1), emitter->particle_time[i]));
        DrawTexturePro
            (
                tex_atlas,
                emitter->atlas_rect,
                (Rectangle){
                    emitter->particle_pos[i].x,
                    emitter->particle_pos[i].y,
                    emitter->atlas_rect.width * emitter->particle_size[i],
                    emitter->atlas_rect.height * emitter->particle_size[i]
                },
                (Vector2){emitter->tex_origin.x * emitter->particle_size[i], emitter->tex_origin.y * emitter->particle_size[i]},
                emitter->particle_rotation[i],
                lerp_colour(emitter->init_colour, emitter->final_colour, emitter->particle_time[i])
            );

        emitter->particle_time[i] += delta / emitter->life_time;
        ++i;
    }
}
