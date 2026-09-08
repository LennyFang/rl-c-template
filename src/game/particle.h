typedef enum Emitter_Shape Emitter_Shape;
enum Emitter_Shape
{
    Shape_Point,
    Shape_Rectangle,
    Shape_Circle,
    Shape_Count
};

typedef union Emitter_Size Emitter_Size;
union Emitter_Size
{
    Vector2 dim;
    struct
    {
        f32 outer_radius;
        f32 inner_radius;
    };
};

typedef struct Emitter Emitter;
struct Emitter
{
    Arena arena;
    Vector2* particle_pos;
    Vector2* particle_vel;
    f32* particle_size;
    f32* particle_rotation;
    f32* particle_time;

    // Set Params
    f32 spawn_rate;
    f32 spawn_amount;
    f32 life_time; // in seconds
    u32 max_particles;
    
    // Rectangle position for the texture on the texture atlas TODO: Add this in
    Rectangle atlas_rect;
    Vector2 tex_origin;

    // Spawner Shape
    Emitter_Shape shape;
    Vector2 pos;
    Emitter_Size size;

    // Runtime Trackers
    f32 timer;
    u32 current_particles;

    // Acceleration
    Vector2 accel;

    // Init Velocity
    Vector2 init_vel_min;
    Vector2 init_vel_max;

    // Init Velocity Dir Normal
    b32 normal_vel;
    f32 init_normal_vel_min;
    f32 init_normal_vel_max;

    // Size TODO: ADD IN LOGIC FOR THIS
    f32 init_size_min;
    f32 init_size_max;

    // Colour & Alpha TODO: yeah idk how to lerp these into a gradient lol
    Color init_colour;
    Color final_colour;

    // Rotation TODO: ADD IN LOGIC FOR THIS
    f32 init_rotation_min;
    f32 init_rotation_max;
    f32 rotation_speed;
};

Emitter init_emitter(Arena* backing_arena, f32 life_time, f32 spawn_rate, u32 spawn_amount);
void destory_emitter(Emitter* emitter);
void uar_emitter(Emitter* emitter, Texture2D tex_atlas, f32 delta);
