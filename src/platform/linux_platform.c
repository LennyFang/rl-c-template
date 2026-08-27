#include "../vendor/raylib/raylib.h"
#include "../base/base.h"
#include "../game/game.h"
#include "platform.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../base/base.c"

#define GAME_CODE_FILE_NAME "./bin/game.so"
#define GAME_CODE_COPY_NAME "./bin/temp_game.so"
#define MOD_CHECK_DURATION 0.25

typedef struct Game_Code Game_Code;
struct Game_Code 
{
    void* game_lib;
    Game_Update_Render_Func* game_update_render;
    i64 lib_mod_time;
    f32 check_delta;
    b8 is_mod_time_changing;
    b8 is_valid;
};

// TODO: Make this less error prone and check for more errors
b32
copy_file(char* src, char* dst) {
    i32 src_handle = open(src, O_RDONLY);
    if (src_handle == -1)
    {
        printf("Unable to open src\n");
        return false;
    }

    i32 dst_handle = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_handle == -1)
    {
        printf("Unable to open dst\n");
        return false;
    }

    u8 buffer[4096];
    i64 read_bytes = 0;
    while(read_bytes = read(src_handle, buffer, 4096 * sizeof(u8)), read_bytes > 0)
    {
        i64 written_bytes = 0;
        do 
        {
            written_bytes += write(dst_handle, buffer + written_bytes, read_bytes - written_bytes);
        } 
        while (written_bytes < read_bytes);
    }

    close(src_handle);
    close(dst_handle);
    return true;
}

i64
check_game_code_mod_time()
{
    struct stat file_info;
    if (stat(GAME_CODE_FILE_NAME, &file_info) == -1)
    {
        // error
        return 0;
    }
    return file_info.st_mtime;
}

Game_Code
load_game_code()
{
    Game_Code game_code = {0};
    if (!copy_file(GAME_CODE_FILE_NAME, GAME_CODE_COPY_NAME))
    {
        printf("Unable to copy game code\n");
    }

    void* game_lib = dlopen(GAME_CODE_COPY_NAME, RTLD_NOW | RTLD_LOCAL);
    if (game_lib != NULL)
    {
        game_code.game_lib = game_lib;
        game_code.game_update_render = (Game_Update_Render_Func*)dlsym(game_lib, "game_update_render");
        game_code.is_valid = (game_code.game_update_render != NULL);
        game_code.lib_mod_time = check_game_code_mod_time();
        printf("Reloading game code...\n");
    } 
    else 
    {
        printf("\nerror %s\n", dlerror());
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
    if (game_code->game_lib != NULL)
    {
        dlclose(game_code->game_lib);
    }
    game_code->is_valid = false;
    game_code->game_update_render = game_update_render_stub;
}

int 
main()
{
    Game_Memory* game_mem = calloc(1, sizeof(Game_Memory));
    Game_Code game_code = load_game_code();

    InitWindow(400, 400, "Test");
    SetTargetFPS(60);
    // TODO: Maybe change this to a custom loop?
    while (!WindowShouldClose()) 
    {
        // NOTE: All this code is just to check the mod time every MOD_CHECK_DURATION to see if the mod time is stablized
        // Since the orginal game code lib is modified mulitple times before fully writen to so reloading right at the first
        // mod time produces errors
        i64 mod_time = check_game_code_mod_time();
        if (!game_code.is_mod_time_changing && mod_time > game_code.lib_mod_time)
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
                if (game_code.lib_mod_time == mod_time)
                {
                    game_code.is_mod_time_changing = false;
                    unload_game_code(&game_code);
                    game_code = load_game_code();
                }
            } 
        }
        
        game_code.game_update_render(game_mem);
    }

    free(game_mem);
}
