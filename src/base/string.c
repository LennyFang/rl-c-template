// cstring helpers

// This function does not include the null terminating character
u64
get_cstring_len(u8* str)
{
    u64 result = 0;
    if (str != NULL){
        for (; *(str+result) != '\0'; result++);
    }
    return result;
}

// String8
String
push_string(Arena* arena, u8* str)
{
    String result = {0};
    result.len = get_cstring_len(str);
    result.data = (u8*)arena_allocate(arena, result.len+1);
    for (u64 i = 0; i < result.len; i++)
    {
        result.data[i] = str[i];
    }
    result.data[result.len] = '\0';
    return result;
}

String
push_stringf(Arena* arena, u8* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    String result = {0};
    result.len = vsnprintf(NULL, 0, fmt, args);
    result.data = (u8*)arena_allocate(arena, result.len+1);
    vsnprintf(result.data, result.len, fmt, args);
    result.data[result.len] = '\0';
    va_end(args);

    return result;
}

String
copy_string(Arena* arena, String str)
{
    String result = {0};
    result.len = str.len;
    result.data = (u8*)arena_allocate(arena, result.len+1);
    
    for (u32 i = 0; (result.data[i] = str.data[i]); i++);

    return result;
}
