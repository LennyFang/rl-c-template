UI_Context
ui_init_context(Arena* backing_arena, UI_Measure_Text_Func* text_func)
{
    UI_Context ctx = {0};
    if (text_func == NULL)
    {
        ctx.ui_measure_text = ui_measure_text_stub;
    } 
    else 
    {
        ctx.ui_measure_text = text_func;
    }

    ctx.build_arena[0] = arena_init(arena_allocate(backing_arena, MB*4), MB*4);
    ctx.build_arena[1] = arena_init(arena_allocate(backing_arena, MB*4), MB*4);
    ctx.hash_arena = arena_init(arena_allocate(backing_arena, MB*4), MB*4);
    return ctx;
}

UI_Block*
ui_new_block(UI_Context* ctx, String text)
{
    UI_Block* block = arena_allocate(get_current_arena(ctx), sizeof(UI_Block));
    *block = (UI_Block){0};

    if (ctx->parent_count == 0)
    {
        block->parent = ctx->root_block;
    }
    else
    {
        block->parent = ctx->parent_stack[ctx->parent_count-1];
    }

    if (block->parent->first != NULL)
    {
        block->prev = block->parent->last;
        block->prev->next = block;
        block->parent->last = block;
    } 
    else 
    {
        block->parent->first = block;
        block->parent->last = block;
    }
    // TODO: Init all the parameters from the style stacks here
    return block;
}

// The root rect will specify the size and position of the ui
void
ui_begin(UI_Context* ctx, UI_Rect root_display_rect, UI_Axis root_child_axis)
{
    // Increment Frame
    ctx->current_frame += 1;

    // Clear Arena
    Arena* current_arena = get_current_arena(ctx);
    arena_clear(get_current_arena(ctx));

    // Setup Root
    UI_Block* block = arena_allocate(current_arena, sizeof(UI_Block));
    *block = (UI_Block){0};
    block->display_rect = root_display_rect;
    block->child_axis = root_child_axis;

    block->semantic_size[UI_Axis_X].value = root_display_rect.w;
    block->semantic_size[UI_Axis_X].strictness = 1.0f;

    block->semantic_size[UI_Axis_Y].value = root_display_rect.h;
    block->semantic_size[UI_Axis_Y].strictness = 1.0f;
    
    // Inputs
}

void
ui_end(UI_Context* ctx)
{
}

internal void
ui_calculate_abs_size(UI_Context* ctx)
{
    UI_Block* current_block = ctx->root_block;
    while (current_block != NULL)
    {
        if (current_block->semantic_size[UI_Axis_X].type == UI_Size_Pixel)
        {
            current_block->computed_size.x = current_block->semantic_size[UI_Axis_X].value;
        }
        else if (current_block->semantic_size[UI_Axis_X].type == UI_Size_Text)
        {
            f32 text_width = ctx->ui_measure_text(current_block->text).x;
            // the *2 is account for padding on both sides
            current_block->computed_size.x = text_width + current_block->semantic_size[UI_Axis_X].value*2;
        }

        if (current_block->semantic_size[UI_Axis_Y].type == UI_Size_Pixel)
        {
            current_block->computed_size.y = current_block->semantic_size[UI_Axis_Y].value;
        }
        else if (current_block->semantic_size[UI_Axis_Y].type == UI_Size_Text)
        {
            f32 text_height = ctx->ui_measure_text(current_block->text).y;
            // the *2 is account for padding on both sides
            current_block->computed_size.y = text_height + current_block->semantic_size[UI_Axis_Y].value*2;
        }

        current_block = cool_traversal(current_block, ctx->root_block).block;
    }
}

internal void
ui_calculate_parent_precent_size(UI_Context* ctx)
{
    UI_Block* current_block = ctx->root_block;
    while (current_block != NULL)
    {
        if (current_block->semantic_size[UI_Axis_X].type == UI_Size_Parent_Precent)
        {
            current_block->computed_size.x = current_block->parent->semantic_size[UI_Axis_X].value * current_block->semantic_size[UI_Axis_X].value;
        }

        if (current_block->semantic_size[UI_Axis_Y].type == UI_Size_Parent_Precent)
        {
            current_block->computed_size.y = current_block->parent->semantic_size[UI_Axis_Y].value * current_block->semantic_size[UI_Axis_Y].value;
        }
        current_block = cool_traversal(current_block, ctx->root_block).block;
    }
}

internal void
ui_calculate_child_sum_size(UI_Context* ctx)
{
    UI_Block* current_block = ctx->root_block;
    while (current_block != NULL)
    {
        UI_Block_Node node = cool_traversal(current_block, ctx->root_block);
        // Ingore the pushes and zero pop count movements because we only care about the pops
        if (node.push_count > 0 || node.pop_count <= 0 )
        {
            current_block = node.block;
            continue;
        }
        // TODO: Handle root case

        node.pop_count -= 1;
        current_block = current_block->prev;
        for (u32 i = 0; i < node.pop_count; i++)
        {
            current_block = current_block->last;
        }

        for (u32 i = 0; i < node.pop_count; i++, current_block = current_block->parent)
        {
            UI_Block* child = current_block->first;
            while (child != NULL) 
            {
                if (current_block->semantic_size[UI_Axis_X].type == UI_Size_Child_Sum)
                {
                    current_block->computed_size.x += child->computed_size.x;
                }

                if (current_block->semantic_size[UI_Axis_Y].type == UI_Size_Child_Sum)
                {
                    current_block->computed_size.y += child->computed_size.y;
                }
                child = child->next;
            }
        }
        current_block = current_block->next;
    }
}

internal void
ui_calculate_size_constraints(UI_Context* ctx)
{
    UI_Block* current_block = ctx->root_block;
    for (; current_block != NULL; current_block = cool_traversal(current_block, ctx->root_block).block)
    {
        // Ignore blocks that do not have children as constraints only matter when there are children
        if (current_block->first == NULL)
        {
            continue;
        }

        UI_Axis non_child_axis = 1 - current_block->child_axis;

        f32 children_sum = 0;
        f32 shavable_sum = 0;

        // First pass to just clamp all the children within the bounds of the non aligned child axis of the parent
        // and to calculate for the amount we are able to shave off
        for (UI_Block* child = current_block->first; child != NULL; child = child->next)
        {
            // TODO: If you create a dynamic array to cache the shavable_amount from each block or have it be in the struct
            children_sum += child->computed_size.v[current_block->child_axis];
            shavable_sum += child->computed_size.v[current_block->child_axis] * (1.0f - child->semantic_size[current_block->child_axis].strictness);
            child->computed_size.v[non_child_axis] = Clamp(0.0, child->computed_size.v[non_child_axis], current_block->computed_size.v[non_child_axis]);
        }

        f32 violation_amount = current_block->computed_size.v[current_block->child_axis] - children_sum;

        f32 ratio = violation_amount / shavable_sum;

        if (violation_amount > 0.0)
        {
            for (UI_Block* child = current_block->first; child != NULL; child = child->next)
            {
                f32 shavable_amount = child->computed_size.v[current_block->child_axis] * (1.0 - child->semantic_size[current_block->child_axis].strictness);
                child->computed_size.v[current_block->child_axis] -= shavable_amount * ratio;

                // Recompute the size for each sub child if they are parent precent
                for (UI_Block* sub_child = child->first; sub_child != NULL; sub_child = sub_child->next)
                {
                    if (sub_child->semantic_size[UI_Axis_X].type == UI_Size_Parent_Precent)
                    {
                        sub_child->computed_size.x = child->semantic_size[UI_Axis_X].value * sub_child->semantic_size[UI_Axis_X].value;
                    }

                    if (sub_child->semantic_size[UI_Axis_Y].type == UI_Size_Parent_Precent)
                    {
                        sub_child->computed_size.y = child->semantic_size[UI_Axis_Y].value * sub_child->semantic_size[UI_Axis_Y].value;
                    }
                }
            }
        }
    }
}

internal void
ui_calculate_positions(UI_Context* ctx)
{
    for (UI_Block* current_block = ctx->root_block; current_block != NULL; current_block = cool_traversal(current_block, ctx->root_block).block)
    {
        UI_Vector2 current_pos = {0};
        for (UI_Block* child = current_block->first; child != NULL; child = child->next)
        {
            child->relative_pos = current_pos;

            current_pos.v[current_block->child_axis] += child->computed_size.v[current_block->child_axis];

            child->display_rect.x = current_block->display_rect.x;
            child->display_rect.y = current_block->display_rect.y;

            child->display_rect.x += child->relative_pos.x;
            child->display_rect.y += child->relative_pos.y;

            child->display_rect.w = current_block->computed_size.x;
            child->display_rect.h = current_block->computed_size.y;
        }
    }
}

// Helpers
internal inline Arena*
get_current_arena(UI_Context* ctx)
{
    return &ctx->build_arena[ctx->current_frame & 1];
}

internal UI_Block_Node
cool_traversal(UI_Block* block, UI_Block* root)
{
    // TODO: Once hashing happens make sure to to check root with something else
    UI_Block_Node result = {0};

    if (block == NULL)
    {
        return result;
    }

    if (block->first != NULL)
    {
        result.push_count = 1;
        result.block = block->first;
        return result;
    } 
    else
    {
        while (block != NULL && block != root)
        {
            if (block->next != NULL)
            {
                result.block = block->next;
                return result;
            } 
            else 
            {
                block = block->parent;
                result.pop_count += 1;
            }
        }
        return result;
    }
}

// Hashing
#define FNV1A_64_OFFSET 14695981039346656037ULL
#define FNV1A_64_PRIME 1099511628211

internal u64
fnv1a_hash(void* data, u64 length)
{
    u8* bytes = data;
    u64 hash = FNV1A_64_OFFSET;
    for (u32 i = 0; i < length; i++)
    {
        hash ^= bytes[i];
        hash *= FNV1A_64_PRIME;
    }
    return hash;
}

internal u64
hash_string(String str)
{
    return fnv1a_hash((void*)str.data, str.len);
}

