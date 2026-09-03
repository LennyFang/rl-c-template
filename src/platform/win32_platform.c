#include "../vendor/raylib/raylib.h"
#include "../base/base.h"
#include "../game/game.h"
#include "platform.h"

#include <windef.h>
#include <winbase.h>
#include <fileapi.h>
#include <libloaderapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "../base/base.c"

#define GAME_CODE_FILE_NAME "./bin/game.dll"
#define GAME_CODE_COPY_NAME "./bin/temp_game.dll"

typedef struct Game_Code Game_Code;
struct Game_Code 
{
    HMODULE game_lib;
    Game_Update_Render_Func* game_update_render;
    FILETIME lib_mod_time;
    f32 check_delta;
    b8 is_mod_time_changing;
    b8 is_valid;
};

FILETIME
check_game_code_mod_time()
{
    WIN32_FIND_DATAA file_info = {0};
    HANDLE file_handle = FindFirstFileA(GAME_CODE_FILE_NAME, &file_info);
    if (file_handle != NULL) {
        // error
        return file_info.ftLastWriteTime;
    }
    FindClose(file_handle);
    return file_info.ftLastWriteTime;
}

Game_Code
load_game_code()
{
    Game_Code game_code = {0};
    // Make a copy of the file
    if (!CopyFile(GAME_CODE_FILE_NAME, GAME_CODE_COPY_NAME, false)) {
        printf("Unable to copy game code\n");
    }
    HMODULE game_lib = LoadLibraryA(GAME_CODE_COPY_NAME);
    if (game_lib != NULL)
    {
        game_code.game_lib = game_lib;
        game_code.game_update_render = (Game_Update_Render_Func*)GetProcAddress(game_lib, "game_update_render");
        game_code.is_valid = (game_code.game_update_render != NULL);
        game_code.lib_mod_time = check_game_code_mod_time();
    } else {
        // WARN: Commenting out these errors because when you rebuild the game code and write to it apparently it doesn't lock the file
        // and dlopen is still able to access the file which is incomplete
        // printf("\nerror %s\n", dlerror());
    }

    if (!game_code.is_valid)
    {
        game_code.game_update_render = game_update_render_stub;
    }
    return game_code;
}

void
unload_game_code(Game_Code* game_code)
{
    if (game_code->game_lib != NULL) {
        FreeLibrary(game_code->game_lib);
    }
    game_code->is_valid = false;
    game_code->game_update_render = game_update_render_stub;
}

int 
main()
{
    Game_Memory* game_mem = calloc(1, sizeof(Game_Memory));
    Game_Code game_code = load_game_code();

    // TODO: Maybe change this to a custom loop?
    InitWindow(400, 400, "Test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // NOTE: All this code is just to check the mod time every MOD_CHECK_DURATION to see if the mod time is stablized
        // Since the orginal game code lib is modified mulitple times before fully writen to so reloading right at the first
        // mod time produces errors
        FILETIME mod_time = check_game_code_mod_time();
        // mod_time > lib_mod_time
        if (!game_code.is_mod_time_changing && CompareFileTime(&mod_time, &game_code.lib_mod_time) == 1)
        {
            game_code.check_delta = 0;
            game_code.is_mod_time_changing = true;
            game_code.lib_mod_time = mod_time;
        }
        else if (game_code.is_mod_time_changing)
        {
            game_code.check_delta += GetFrameTime();
            if (game_code.check_delta > MOD_CHECK_DURATION)
            {
                if (CompareFileTime(&mod_time, &game_code.lib_mod_time) == 0)
                {
                    game_code.is_mod_time_changing = false;
                    unload_game_code(&game_code);
                    game_code = load_game_code();
                }
            } 
        }
    }

    free(game_mem);
}
