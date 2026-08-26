#include "../vendor/raylib/raylib.h"
#include "../vendor/raylib/raymath.h"
#include "../base/base.h"

typedef struct Game_Memory Game_Memory;
struct Game_Memory
{
    Vector2 player_pos;
    b32 toggle;
};

// Something cool I saw in HMH where you have dynamically loaded functions
// and you create a stub for it so your game doesn't crash
#define GAME_UPDATE_RENDER(name) void name(Game_Memory* mem)
typedef GAME_UPDATE_RENDER(Game_Update_Render_Func);
GAME_UPDATE_RENDER(game_update_render_stub)
{
};

