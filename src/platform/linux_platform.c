#include "../vendor/raylib/raylib.h"
#include "../vendor/raylib/raymath.h"
#include "../vendor/raylib/rlgl.h"
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

// NOTE: This might need to be removed because you can do the samething without copying the files on my system at least
b32
copy_file(char* src, char* dst) {
    i32 src_handle = open(src, O_RDONLY);
    if (src_handle == -1) {
        printf("Unable to open src\n");
        return false;
    }

    i32 dst_handle = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_handle == -1) {
        printf("Unable to open dst\n");
        return false;
    }

    char buffer[4096];
    i64 read_bytes = 0;
    while((read_bytes = read(src_handle, buffer, 4096 * sizeof(char))) > 0) {
        i64 written_bytes = 0;
        do {
            written_bytes += write(dst_handle, buffer + written_bytes, read_bytes - written_bytes);
        } while (written_bytes < read_bytes);
    }

    close(src_handle);
    close(dst_handle);
    return true;
}

i64
check_game_code_mod_time()
{
    struct stat file_info;
    if (stat(GAME_CODE_FILE_NAME, &file_info) == -1) {
        // error
        return 0;
    }
    return file_info.st_mtime;
}

Game_Code
load_game_code()
{
    Game_Code game_code = {0};
    // Make a copy of the file
    if (!copy_file(GAME_CODE_FILE_NAME, GAME_CODE_COPY_NAME)) {
        printf("Unable to copy game code\n");
    }
    void* game_lib = dlopen(GAME_CODE_COPY_NAME, RTLD_NOW | RTLD_LOCAL);
    if (game_lib != NULL)
    {
        game_code.game_lib = game_lib;
        game_code.game_update_render = (Game_Update_Render_Func*)dlsym(game_lib, "game_update_render");
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

    // TODO: Maybe change this to a custom loop?
    InitWindow(400, 400, "Test");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        i64 mod_time = check_game_code_mod_time();
        if (mod_time > game_code.lib_mod_time) {
            unload_game_code(&game_code);
            game_code = load_game_code();
        }
        game_code.game_update_render(game_mem);
    }

    free(game_mem);
}
