typedef struct Game_Code Game_Code;
struct Game_Code 
{
    void* game_lib;
    Game_Update_Render_Func* game_update_render;
    i64 lib_mod_time;
    b8 is_valid;
};
