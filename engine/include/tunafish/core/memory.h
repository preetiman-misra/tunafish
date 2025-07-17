//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

// Memory alignment (must be power of 2)
#define TF_MEMORY_ALIGNMENT 16

// Memory allocator types
typedef enum {
    TF_ALLOCATOR_TYPE_ARENA = 0,
    TF_ALLOCATOR_TYPE_POOL = 1,
    TF_ALLOCATOR_TYPE_STACK = 2,
    TF_ALLOCATOR_TYPE_COUNT
} TF_AllocatorType;

// Forward declarations
typedef struct TF_Allocator TF_Allocator;
typedef struct TF_Arena TF_Arena;
typedef struct TF_Pool TF_Pool;
typedef struct TF_Stack TF_Stack;

// Allocation function pointers
typedef void * (*TF_AllocFunc)(TF_Allocator *allocator, usize size, usize alignment);

typedef void (*TF_FreeFunc)(TF_Allocator *allocator, void *ptr);

typedef void (*TF_ClearFunc)(TF_Allocator *allocator);

// Base allocator interface
struct TF_Allocator {
    TF_AllocatorType type;
    TF_AllocFunc alloc;
    TF_FreeFunc free;
    TF_ClearFunc clear;
    void *data;
    const char *name;
};

// Arena allocator - Linear allocation, no individual frees
struct TF_Arena {
    TF_Allocator base;
    u8 *memory;
    usize size;
    usize offset;
    usize peak_usage;
};

// Pool allocator - Fixed-size blocks, fast alloc/free
struct TF_Pool {
    TF_Allocator base;
    u8 *memory;
    usize total_size;
    usize block_size;
    usize block_count;
    void **free_list;
    usize free_count;
    usize peak_usage;
};

// Stack allocator - LIFO allocation with markers
struct TF_Stack {
    TF_Allocator base;
    u8 *memory;
    usize size;
    usize offset;
    usize peak_usage;
    usize markers[32]; // Support 32 nested scopes
    u32 marker_count;
};

// Memory statistics
typedef struct {
    usize total_allocated;
    usize peak_allocated;
    usize current_allocated;
    usize allocation_count;
    usize free_count;
    usize arena_count;
    usize pool_count;
    usize stack_count;
} TF_MemoryStats;

// Core memory system API
TF_API void tf_memory_init(void);

TF_API void tf_memory_shutdown(void);

TF_API TF_MemoryStats tf_memory_get_stats(void);

TF_API void tf_memory_print_stats(void);

// Default allocator (uses system malloc/free)
TF_API TF_Allocator *tf_memory_get_default_allocator(void);

// Generic allocator interface
TF_API void *tf_alloc(TF_Allocator *allocator, usize size);

TF_API void *tf_alloc_aligned(TF_Allocator *allocator, usize size, usize alignment);

TF_API void tf_free(TF_Allocator *allocator, void *ptr);

TF_API void tf_clear(TF_Allocator *allocator);

// Arena allocator
TF_API TF_Arena *tf_arena_create(const char *name, usize size);

TF_API void tf_arena_destroy(TF_Arena *arena);

TF_API void *tf_arena_alloc(TF_Arena *arena, usize size);

TF_API void *tf_arena_alloc_aligned(TF_Arena *arena, usize size, usize alignment);

TF_API void tf_arena_clear(TF_Arena *arena);

TF_API usize tf_arena_get_usage(TF_Arena *arena);

TF_API usize tf_arena_get_peak_usage(TF_Arena *arena);

// Pool allocator
TF_API TF_Pool *tf_pool_create(const char *name, usize block_size, usize block_count);

TF_API void tf_pool_destroy(TF_Pool *pool);

TF_API void *tf_pool_alloc(TF_Pool *pool);

TF_API void tf_pool_free(TF_Pool *pool, void *ptr);

TF_API usize tf_pool_get_usage(TF_Pool *pool);

TF_API usize tf_pool_get_free_count(TF_Pool *pool);

// Stack allocator
TF_API TF_Stack *tf_stack_create(const char *name, usize size);

TF_API void tf_stack_destroy(TF_Stack *stack);

TF_API void *tf_stack_alloc(TF_Stack *stack, usize size);

TF_API void *tf_stack_alloc_aligned(TF_Stack *stack, usize size, usize alignment);

TF_API void tf_stack_push_marker(TF_Stack *stack);

TF_API void tf_stack_pop_marker(TF_Stack *stack);

TF_API void tf_stack_clear(TF_Stack *stack);

TF_API usize tf_stack_get_usage(TF_Stack *stack);

// Utility macros
#define TF_ALLOC(allocator, type) \
    ((type*)tf_alloc((allocator), sizeof(type)))

#define TF_ALLOC_ARRAY(allocator, type, count) \
    ((type*)tf_alloc((allocator), sizeof(type) * (count)))

#define TF_ARENA_ALLOC(arena, type) \
    ((type*)tf_arena_alloc((arena), sizeof(type)))

#define TF_ARENA_ALLOC_ARRAY(arena, type, count) \
    ((type*)tf_arena_alloc((arena), sizeof(type) * (count)))

#define TF_POOL_ALLOC(pool, type) \
    ((type*)tf_pool_alloc(pool))

#define TF_STACK_ALLOC(stack, type) \
    ((type*)tf_stack_alloc((stack), sizeof(type)))

#define TF_STACK_ALLOC_ARRAY(stack, type, count) \
    ((type*)tf_stack_alloc((stack), sizeof(type) * (count)))

// Memory alignment utilities
#define TF_ALIGN_UP(value, alignment) \
    (((value) + (alignment) - 1) & ~((alignment) - 1))

#define TF_ALIGN_DOWN(value, alignment) \
    ((value) & ~((alignment) - 1))

#define TF_IS_ALIGNED(value, alignment) \
    (((value) & ((alignment) - 1)) == 0)

// Memory debugging (only in debug builds)
#if TF_IS_DEBUG
#define TF_MEMORY_TRACK_ALLOCATIONS 1
TF_API void tf_memory_debug_print_allocations(void);

TF_API void tf_memory_debug_check_leaks(void);
#else
    #define TF_MEMORY_TRACK_ALLOCATIONS 0
#endif

#ifdef __cplusplus
}
#endif
