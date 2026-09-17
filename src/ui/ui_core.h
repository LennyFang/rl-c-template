// I amma just say it here but my UI system will probably look like Ryan Fleury's
// because I am following his blog while also referencing the raddebugger source code
//
// Although this is a raylib template, I plan to make this UI backend not dependant on stuff like Vector2 and Rectangle

#define MAX_PARENTS 32
#define MAX_BLOCKS 4096

typedef u32 UI_Feature_Flag;
typedef u64 UI_Hash_Key;

typedef struct UI_Colour UI_Colour;
struct UI_Colour
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

typedef enum UI_Axis UI_Axis;
enum UI_Axis
{
    UI_Axis_X,
    UI_Axis_Y,
    UI_Axis_Count
};

typedef enum UI_Size_Type UI_Size_Type;
enum UI_Size_Type
{
    UI_Size_Pixel,
    UI_Size_Text,
    UI_Size_Parent_Precent,
    UI_Size_Child_Sum,
};

typedef union UI_Vector2 UI_Vector2;
union UI_Vector2
{
    struct
    {
        f32 x;
        f32 y;
    };
    f32 v[UI_Axis_Count];
};

typedef struct UI_Semantic_Size UI_Semantic_Size;
struct UI_Semantic_Size
{
    UI_Size_Type type;
    f32 value;
    f32 strictness;
};

typedef union UI_Rect UI_Rect;
union UI_Rect
{
    struct
    {
        f32 pos[UI_Axis_Count];
        f32 dim[UI_Axis_Count];
    };
    struct
    {
        f32 x;
        f32 y;
        f32 w;
        f32 h;
    };
};

#define UI_MEASURE_TEXT(name) UI_Vector2 name(String str)
typedef UI_MEASURE_TEXT(UI_Measure_Text_Func);
UI_MEASURE_TEXT(ui_measure_text_stub)
{
    return (UI_Vector2){0};
};

typedef struct UI_Block UI_Block;
struct UI_Block
{
    // Hashing
    UI_Hash_Key hash_key;
    UI_Block* hash_next;
    UI_Block* hash_prev;

    // Connections
    UI_Block* first;
    UI_Block* last;
    UI_Block* next;
    UI_Block* prev;
    UI_Block* parent;

    // Text
    String text;
    UI_Vector2 text_pos;

    // Size & Position
    UI_Semantic_Size semantic_size[UI_Axis_Count];
    UI_Vector2 relative_pos;
    UI_Vector2 computed_size;
    UI_Rect display_rect;

    // Children Layout
    UI_Axis child_axis;

    // Colours
    UI_Colour background_colour;
    UI_Colour border_colour;
    UI_Colour text_colour;

    // Flags
    UI_Feature_Flag feature_flags;
};

typedef struct UI_Context UI_Context;
struct UI_Context
{
    Arena build_arena[2];
    Arena hash_arena;
    UI_Block* hash_table;
    u64 hash_table_size;
    UI_Measure_Text_Func* ui_measure_text;
    UI_Block* root_block;
    UI_Block* parent_stack[MAX_PARENTS];
    u32 parent_count;
    u32 current_frame;
};

typedef struct UI_Block_Node UI_Block_Node;
struct UI_Block_Node
{
    UI_Block* block;
    u32 push_count;
    u32 pop_count;
};

// Hashing
internal u64 fnv1a_hash(void* data, u64 length);
internal u64 hash_string(String str);

// Helpers
internal inline Arena* get_current_arena(UI_Context* ctx);
internal UI_Block_Node cool_traversal(UI_Block* block, UI_Block* root);

// UI
UI_Context ui_init_context(Arena* backing_arena, UI_Measure_Text_Func* text_func);
UI_Block* ui_new_block(UI_Context* ctx, String text);
void ui_begin(UI_Context* ctx, UI_Rect root_display_rect, UI_Axis root_child_axis);
void ui_end(UI_Context* ctx);

// Layout calculation
internal void ui_calculate_abs_size(UI_Context* ctx);
internal void ui_calculate_parent_precent_size(UI_Context* ctx);
internal void ui_calculate_child_sum_size(UI_Context* ctx);
internal void ui_calculate_size_constraints(UI_Context* ctx);
internal void ui_calculate_positions(UI_Context* ctx);
