#include "game.h"

#define SPEED 10

void 
handle_input(Game_Memory* mem)
{
    mem->input_state = 0;
    if (IsKeyDown(KEY_W)) mem->input_state |= Input_Move_Up;
    if (IsKeyDown(KEY_S)) mem->input_state |= Input_Move_Down;
    if (IsKeyDown(KEY_A)) mem->input_state |= Input_Move_Left;
    if (IsKeyDown(KEY_D)) mem->input_state |= Input_Move_Right;
}

inline b32
check_input_down(Game_Memory* mem, enum Semantic_Input input)
{
    return mem->input_state & input;
}

INIT_GAME_MEMORY(init_game_memory)
{
    mem->tex_atlas = LoadTexture("./assets/atlas.png");
    f32 life_time = 1.50f;
    f32 spawn_rate = 1.50f;
    u32 spawn_amount = 250;
    mem->emitter = init_emitter(&mem->temp_arena, life_time, spawn_rate, spawn_amount);
    mem->emitter.shape = Shape_Circle;
    mem->emitter.pos = (Vector2){200, 100};
    mem->emitter.size.outer_radius = 50;
    mem->emitter.size.inner_radius = 10;
    // mem->emitter.size.dim = (Vector2){400, 25};
    // mem->emitter.init_vel_min = (Vector2){100, 0};
    // mem->emitter.init_vel_max = (Vector2){200, 0};
    mem->emitter.init_normal_vel_min = 60;
    mem->emitter.init_normal_vel_max = 100;
    mem->emitter.normal_vel = true;

    mem->emitter.init_vel_min = (Vector2){25, 1000};
    mem->emitter.init_vel_max = (Vector2){75, 1250};
    mem->emitter.accel = (Vector2){0, 150};

    mem->emitter.atlas_rect = (Rectangle){0, 0, 4, 4};
    mem->emitter.init_size_min = 1.0f;
    mem->emitter.init_size_max = 2.0f;

    mem->emitter.init_rotation_min = 0.0;
    mem->emitter.init_rotation_max = 360.0;
    mem->emitter.rotation_speed = 500.0;
    
    mem->emitter.init_colour = SKYBLUE;
    mem->emitter.final_colour = ColorAlpha(RED, 0.1);
    mem->emitter.tex_origin = (Vector2){2, 2};
}

GAME_UPDATE_RENDER(game_update_render)
{
    handle_input(mem);

    if (check_input_down(mem, Input_Move_Up))
    {
        mem->player_pos.y -= SPEED;
    }

    if (check_input_down(mem, Input_Move_Down))
    {
        mem->player_pos.y += SPEED;
    }

    if (check_input_down(mem, Input_Move_Left))
    {
        mem->player_pos.x -= SPEED;
    }

    if (check_input_down(mem, Input_Move_Right))
    {
        mem->player_pos.x += SPEED;
    }

    BeginDrawing();
    ClearBackground(DARKGRAY);
    mem->emitter.pos = mem->player_pos;
    DrawCircleV(mem->player_pos, 4, RED);
    uar_emitter(&mem->emitter, mem->tex_atlas, GetFrameTime());
    EndDrawing();
}
