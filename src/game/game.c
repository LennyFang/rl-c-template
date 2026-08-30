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
    ClearBackground(RAYWHITE);
    DrawCircleV(mem->player_pos, 16, GREEN);
    EndDrawing();
}
