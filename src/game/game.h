#include "../vendor/raylib/raylib_includes.h"
#include <stdio.h>
#include <stdlib.h>
#include "../base/base_includes.h"
#include "particle.h"

#include "../base/base_includes.c"
#include "particle.c"


enum Semantic_Input
{
    Input_Move_Up = 1<<0,
    Input_Move_Down = 1<<1,
    Input_Move_Left = 1<<2,
    Input_Move_Right = 1<<3
};

// This is where the we will toggle the semantic state of the inputs for the game
// from any given input device
typedef u32 Input_State;

typedef struct Game_Memory Game_Memory;
struct Game_Memory
{
    Input_State input_state;
    Vector2 player_pos;
    b32 toggle;

    Emitter emitter;

    Arena persist_arena;
    Arena temp_arena;
};

void handle_input(Game_Memory* mem);
// Something cool I saw in HMH where you have dynamically loaded functions
// and you create a stub for it so your game doesn't crash
#define GAME_UPDATE_RENDER(name) void name(Game_Memory* mem)
typedef GAME_UPDATE_RENDER(Game_Update_Render_Func);
GAME_UPDATE_RENDER(game_update_render_stub)
{
};

#define INIT_GAME_MEMORY(name) void name(Game_Memory* mem)
typedef INIT_GAME_MEMORY(Init_Game_Memory_Func);
INIT_GAME_MEMORY(init_game_memory_stub)
{
};

