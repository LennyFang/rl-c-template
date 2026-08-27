#include <stdint.h>

#define internal static
#define global static

// Types Defs
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

// Max & Min Macros
#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define Assert(Expression) if (!(Expression)) {*((i32*)0) = 0;}
