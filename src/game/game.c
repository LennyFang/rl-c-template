#include "game.h"

#define SPEED 10

GAME_UPDATE_RENDER(game_update_render)
{
    if (!mem->toggle) {
        mem->player_pos.x += SPEED;
        mem->player_pos.y += SPEED;
        if (mem->player_pos.x >= 400) {
            mem->toggle = true;
        }
    } else {
        mem->player_pos.x -= SPEED;
        mem->player_pos.y -= SPEED;
        if (mem->player_pos.x <= 0) {
            mem->toggle = false;
        }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawCircleV(mem->player_pos, 8, RED);
    DrawCircleV(mem->player_pos, 8, BLUE);
    EndDrawing();
}
