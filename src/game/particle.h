typedef struct Emitter Emitter;
struct Emitter {
    Arena arena;
    Vector2* pos;
    Vector2* vel;
    f32* time;

    // Set Params
    f32 spawn_rate;
    f32 spawn_amount;
    f32 life_time; // in seconds
    u32 max_particles;

    // Runtime Trackers
    f32 timer;
    u32 current_particles;
};

Emitter init_emitter(Arena* backing_arena, f32 life_time, f32 spawn_rate, u32 spawn_amount);
void destory_emitter(Emitter* emitter);
void uar_emitter(Emitter* emitter, f32 delta);
