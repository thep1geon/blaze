#ifndef __DEFINES_H
#define __DEFINES_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float   f32;
typedef double  f64;

typedef u8      bool;

#define true    1
#define false   0

#ifdef __LP64__
typedef u64 usize;
typedef i64 isize;
#else
typedef u32 usize;
typedef i32 isize;
#endif

#endif  //__DEFINES_H
