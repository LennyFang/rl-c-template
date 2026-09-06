typedef enum Emitter_Shape Emitter_Shape;
enum Emitter_Shape
{
    Shape_Point,
    Shape_Rectangle,
    Shape_Circle,
    Shape_Count
};

typedef struct Emitter Emitter;
struct Emitter
{
    Arena arena;
    Vector2* particle_pos;
    Vector2* particle_vel;
    f32* particle_time;

    // Set Params
    f32 spawn_rate;
    f32 spawn_amount;
    f32 life_time; // in seconds
    u32 max_particles;

    // Spawner Shape
    Emitter_Shape shape;
    Vector2 pos;

    // Runtime Trackers
    f32 timer;
    u32 current_particles;

    // Acceleration
    Vector2 accel;

    // Init Velocity
    Vector2 init_vel_min;
    Vector2 init_vel_max;

    // Size
    f32 init_size_min;
    f32 init_size_max;
    // f32 final_size;

    // Colour TODO: yeah idk how to lerp these into a gradient lol
    Color init_colour;
    Color final_colour;
};

Emitter init_emitter(Arena* backing_arena, f32 life_time, f32 spawn_rate, u32 spawn_amount);
void destory_emitter(Emitter* emitter);
void uar_emitter(Emitter* emitter, f32 delta);
