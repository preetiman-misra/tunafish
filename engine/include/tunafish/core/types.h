//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Core integer types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// Floating point types
typedef float f32;
typedef double f64;

// Boolean types
typedef i32 b32;
typedef u8 b8;

// Size types
typedef u64 usize;
typedef i64 isize;

// Constants
#define TF_TRUE 1
#define TF_FALSE 0
#define TF_NULL ((void*)0)

// Utility macros
#define TF_ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0])
#define TF_KILOBYTES(value) ((value) *1024LL)
#define TF_MEGABYTES(value)   (TF_KILOBYTES(value) * 1024LL)
#define TF_GIGABYTES(value)   (TF_MEGABYTES(value) * 1024LL)

// Compile-time assertions
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define TF_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define TF_STATIC_ASSERT(cond, msg) typedef char tf_static_assert_##__LINE__[(cond) ? 1 : -1]
#endif


// Verify type sizes
TF_STATIC_ASSERT(sizeof(u8) == 1, "u8 must be 1 byte");
TF_STATIC_ASSERT(sizeof(u16) == 2, "u16 must be 2 bytes");
TF_STATIC_ASSERT(sizeof(u32) == 4, "u32 must be 4 bytes");
TF_STATIC_ASSERT(sizeof(u64) == 8, "u64 must be 8 bytes");
TF_STATIC_ASSERT(sizeof(f32) == 4, "f32 must be 4 bytes");
TF_STATIC_ASSERT(sizeof(f64) == 8, "f64 must be 8 bytes");

#ifdef __cplusplus
}
#endif
