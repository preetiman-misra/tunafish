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

// =============================================================================
// Arena allocator - Linear allocation, bulk clear only
// =============================================================================
typedef struct TF_Arena {
    u8 *memory;
    usize size;
    usize offset;
    usize peak_usage;
#if TF_IS_DEBUG
    const char *name;
#endif
} TF_Arena;

// =============================================================================
// Pool allocator - Fixed-size blocks with intrusive free list
// =============================================================================
typedef struct TF_Pool {
    u8 *memory;
    usize block_size;
    usize block_count;
    void *free_head;      // Head of intrusive free list (stored in free blocks)
    usize free_count;
    usize peak_usage;
#if TF_IS_DEBUG
    const char *name;
#endif
} TF_Pool;

// =============================================================================
// Stack allocator - LIFO allocation with markers
// =============================================================================
typedef struct TF_Stack {
    u8 *memory;
    usize size;
    usize offset;
    usize peak_usage;
    usize markers[32];
    u32 marker_count;
#if TF_IS_DEBUG
    const char *name;
#endif
} TF_Stack;

// =============================================================================
// Memory statistics
// =============================================================================
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

// =============================================================================
// Core memory system API
// =============================================================================
TF_API void tf_memory_init(void);
TF_API void tf_memory_shutdown(void);
TF_API TF_MemoryStats tf_memory_get_stats(void);
TF_API void tf_memory_print_stats(void);

// =============================================================================
// Arena allocator API
// =============================================================================
TF_API TF_Arena *tf_arena_create(const char *name, usize size);
TF_API void tf_arena_destroy(TF_Arena *arena);
TF_API void *tf_arena_alloc(TF_Arena *arena, usize size);
TF_API void *tf_arena_alloc_aligned(TF_Arena *arena, usize size, usize alignment);
TF_API void tf_arena_clear(TF_Arena *arena);
TF_API usize tf_arena_get_usage(TF_Arena *arena);
TF_API usize tf_arena_get_peak_usage(TF_Arena *arena);

// =============================================================================
// Pool allocator API
// =============================================================================
TF_API TF_Pool *tf_pool_create(const char *name, usize block_size, usize block_count);
TF_API void tf_pool_destroy(TF_Pool *pool);
TF_API void *tf_pool_alloc(TF_Pool *pool);
TF_API void tf_pool_free(TF_Pool *pool, void *ptr);
TF_API usize tf_pool_get_usage(TF_Pool *pool);
TF_API usize tf_pool_get_free_count(TF_Pool *pool);

// =============================================================================
// Stack allocator API
// =============================================================================
TF_API TF_Stack *tf_stack_create(const char *name, usize size);
TF_API void tf_stack_destroy(TF_Stack *stack);
TF_API void *tf_stack_alloc(TF_Stack *stack, usize size);
TF_API void *tf_stack_alloc_aligned(TF_Stack *stack, usize size, usize alignment);
TF_API void tf_stack_push_marker(TF_Stack *stack);
TF_API void tf_stack_pop_marker(TF_Stack *stack);
TF_API void tf_stack_clear(TF_Stack *stack);
TF_API usize tf_stack_get_usage(TF_Stack *stack);

// =============================================================================
// Convenience macros
// =============================================================================
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

// =============================================================================
// Alignment utilities
// =============================================================================
#define TF_ALIGN_UP(value, alignment) \
    (((value) + (alignment) - 1) & ~((alignment) - 1))

#define TF_ALIGN_DOWN(value, alignment) \
    ((value) & ~((alignment) - 1))

#define TF_IS_ALIGNED(value, alignment) \
    (((value) & ((alignment) - 1)) == 0)

// =============================================================================
// Debug utilities
// =============================================================================
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
