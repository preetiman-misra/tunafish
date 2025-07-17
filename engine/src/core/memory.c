//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/memory.h"
#include "tunafish/core/log.h"
#include <stdlib.h>
#include <string.h>

// Global memory state
static struct {
    b32 initialized;
    TF_MemoryStats stats;
    TF_Allocator default_allocator;
    // Debug tracking (only in debug builds)
#if TF_MEMORY_TRACK_ALLOCATIONS
    struct {
        void *ptr;
        usize size;
        const char *file;
        u32 line;
    } allocations[4096];

    u32 allocation_count;
#endif
} s_memory_state = {0};

// Utility functions
static usize tf_memory_align_size(usize size, usize alignment) {
    return TF_ALIGN_UP(size, alignment);
}

static b32 tf_memory_is_power_of_two(usize value) {
    return value != 0 && (value & (value - 1)) == 0;
}

// Default allocator implementation (system malloc/free wrapper)
static void *tf_default_alloc(TF_Allocator *allocator, usize size, usize alignment) {
    (void) allocator; // Unused
    (void) alignment; // System malloc doesn't guarantee specific alignment

    if (size == 0) {
        return TF_NULL;
    }

    void *ptr = malloc(size);
    if (ptr) {
        s_memory_state.stats.total_allocated += size;
        s_memory_state.stats.current_allocated += size;
        s_memory_state.stats.allocation_count++;

        if (s_memory_state.stats.current_allocated > s_memory_state.stats.peak_allocated) {
            s_memory_state.stats.peak_allocated = s_memory_state.stats.current_allocated;
        }
    }

    return ptr;
}

static void tf_default_free(TF_Allocator *allocator, void *ptr) {
    (void) allocator; // Unused

    if (ptr) {
        free(ptr);
        s_memory_state.stats.free_count++;
        // Note: We can't track the exact size freed with system malloc
    }
}

static void tf_default_clear(TF_Allocator *allocator) {
    (void) allocator; // Unused
    TF_WARN("Cannot clear default system allocator");
}

// Arena allocator implementation
static void *tf_arena_alloc_func(TF_Allocator *allocator, usize size, usize alignment) {
    TF_Arena *arena = (TF_Arena *) allocator;
    return tf_arena_alloc_aligned(arena, size, alignment);
}

static void tf_arena_free_func(TF_Allocator *allocator, void *ptr) {
    (void) allocator; // Unused
    (void) ptr; // Unused
    // Arena allocator doesn't support individual frees
}

static void tf_arena_clear_func(TF_Allocator *allocator) {
    TF_Arena *arena = (TF_Arena *) allocator;
    tf_arena_clear(arena);
}

// Pool allocator implementation
static void *tf_pool_alloc_func(TF_Allocator *allocator, usize size, usize alignment) {
    TF_Pool *pool = (TF_Pool *) allocator;
    (void) size; // Pool has fixed block size
    (void) alignment; // Pool blocks are pre-aligned
    return tf_pool_alloc(pool);
}

static void tf_pool_free_func(TF_Allocator *allocator, void *ptr) {
    TF_Pool *pool = (TF_Pool *) allocator;
    tf_pool_free(pool, ptr);
}

static void tf_pool_clear_func(TF_Allocator *allocator) {
    (void) allocator; // Unused - pools don't support clearing
    TF_WARN("Pool allocator doesn't support clearing");
}

// Stack allocator implementation
static void *tf_stack_alloc_func(TF_Allocator *allocator, usize size, usize alignment) {
    TF_Stack *stack = (TF_Stack *) allocator;
    return tf_stack_alloc_aligned(stack, size, alignment);
}

static void tf_stack_free_func(TF_Allocator *allocator, void *ptr) {
    (void) allocator; // Unused
    (void) ptr; // Unused
    // Stack allocator only supports LIFO via markers
}

static void tf_stack_clear_func(TF_Allocator *allocator) {
    TF_Stack *stack = (TF_Stack *) allocator;
    tf_stack_clear(stack);
}

// Core memory system API
TF_API void tf_memory_init(void) {
    if (s_memory_state.initialized) {
        TF_WARN("Memory system already initialized");
        return;
    }

    TF_DEBUG("Initializing memory system...");

    // Initialize statistics
    s_memory_state.stats.total_allocated = 0;
    s_memory_state.stats.peak_allocated = 0;
    s_memory_state.stats.current_allocated = 0;
    s_memory_state.stats.allocation_count = 0;
    s_memory_state.stats.free_count = 0;
    s_memory_state.stats.arena_count = 0;
    s_memory_state.stats.pool_count = 0;
    s_memory_state.stats.stack_count = 0;

    // Initialize default allocator
    s_memory_state.default_allocator.type = TF_ALLOCATOR_TYPE_ARENA; // Actually system malloc
    s_memory_state.default_allocator.alloc = tf_default_alloc;
    s_memory_state.default_allocator.free = tf_default_free;
    s_memory_state.default_allocator.clear = tf_default_clear;
    s_memory_state.default_allocator.data = TF_NULL;
    s_memory_state.default_allocator.name = "default";

    s_memory_state.initialized = TF_TRUE;
    TF_INFO("Memory system initialized");
}

TF_API void tf_memory_shutdown(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return;
    }

    TF_DEBUG("Shutting down memory system...");

    // Print final statistics
    tf_memory_print_stats();

#if TF_MEMORY_TRACK_ALLOCATIONS
    if (s_memory_state.allocation_count > 0) {
        TF_WARN("Memory leaks detected: %u allocations not freed", s_memory_state.allocation_count);
        tf_memory_debug_print_allocations();
    }
#endif

    s_memory_state.initialized = TF_FALSE;
    TF_INFO("Memory system shutdown");
}

TF_API TF_MemoryStats tf_memory_get_stats(void) {
    return s_memory_state.initialized ? s_memory_state.stats : (TF_MemoryStats){0};
}

TF_API void tf_memory_print_stats(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return;
    }

    TF_MemoryStats stats = s_memory_state.stats;
    TF_INFO("Memory Statistics:");
    TF_INFO("  Total allocated: %llu bytes (%.2f MB)",
            (unsigned long long)stats.total_allocated,
            (f32)stats.total_allocated / (1024.0f * 1024.0f));
    TF_INFO("  Peak allocated: %llu bytes (%.2f MB)",
            (unsigned long long)stats.peak_allocated,
            (f32)stats.peak_allocated / (1024.0f * 1024.0f));
    TF_INFO("  Current allocated: %llu bytes (%.2f MB)",
            (unsigned long long)stats.current_allocated,
            (f32)stats.current_allocated / (1024.0f * 1024.0f));
    TF_INFO("  Allocations: %llu", (unsigned long long)stats.allocation_count);
    TF_INFO("  Frees: %llu", (unsigned long long)stats.free_count);
    TF_INFO("  Arenas: %llu", (unsigned long long)stats.arena_count);
    TF_INFO("  Pools: %llu", (unsigned long long)stats.pool_count);
    TF_INFO("  Stacks: %llu", (unsigned long long)stats.stack_count);
}

TF_API TF_Allocator *tf_memory_get_default_allocator(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return TF_NULL;
    }
    return &s_memory_state.default_allocator;
}

// Generic allocator interface
TF_API void *tf_alloc(TF_Allocator *allocator, usize size) {
    return tf_alloc_aligned(allocator, size, TF_MEMORY_ALIGNMENT);
}

TF_API void *tf_alloc_aligned(TF_Allocator *allocator, usize size, usize alignment) {
    if (!allocator || !allocator->alloc) {
        TF_ERROR("Invalid allocator");
        return TF_NULL;
    }

    if (size == 0) {
        return TF_NULL;
    }

    if (!tf_memory_is_power_of_two(alignment)) {
        TF_ERROR("Alignment must be power of two");
        return TF_NULL;
    }

    return allocator->alloc(allocator, size, alignment);
}

TF_API void tf_free(TF_Allocator *allocator, void *ptr) {
    if (!allocator || !allocator->free) {
        TF_ERROR("Invalid allocator");
        return;
    }

    if (ptr) {
        allocator->free(allocator, ptr);
    }
}

TF_API void tf_clear(TF_Allocator *allocator) {
    if (!allocator || !allocator->clear) {
        TF_ERROR("Invalid allocator");
        return;
    }

    allocator->clear(allocator);
}

// Arena allocator implementation
TF_API TF_Arena *tf_arena_create(const char *name, usize size) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return TF_NULL;
    }

    if (size == 0) {
        TF_ERROR("Arena size cannot be zero");
        return TF_NULL;
    }

    TF_DEBUG("Creating arena '%s' with size %llu bytes",
             name ? name : "unnamed", (unsigned long long)size);

    // Allocate arena structure
    TF_Arena *arena = (TF_Arena *) malloc(sizeof(TF_Arena));
    if (!arena) {
        TF_ERROR("Failed to allocate arena structure");
        return TF_NULL;
    }

    // Allocate arena memory
    arena->memory = (u8 *) malloc(size);
    if (!arena->memory) {
        TF_ERROR("Failed to allocate arena memory");
        free(arena);
        return TF_NULL;
    }

    // Initialize arena
    arena->base.type = TF_ALLOCATOR_TYPE_ARENA;
    arena->base.alloc = tf_arena_alloc_func;
    arena->base.free = tf_arena_free_func;
    arena->base.clear = tf_arena_clear_func;
    arena->base.data = arena;
    arena->base.name = name;
    arena->size = size;
    arena->offset = 0;
    arena->peak_usage = 0;

    s_memory_state.stats.arena_count++;

    TF_INFO("Arena '%s' created successfully", name ? name : "unnamed");
    return arena;
}

TF_API void tf_arena_destroy(TF_Arena *arena) {
    if (!arena) {
        TF_WARN("Attempted to destroy null arena");
        return;
    }

    TF_DEBUG("Destroying arena '%s'", arena->base.name ? arena->base.name : "unnamed");

    if (arena->memory) {
        free(arena->memory);
    }

    free(arena);

    s_memory_state.stats.arena_count--;
    TF_INFO("Arena destroyed");
}

TF_API void *tf_arena_alloc(TF_Arena *arena, usize size) {
    return tf_arena_alloc_aligned(arena, size, TF_MEMORY_ALIGNMENT);
}

TF_API void *tf_arena_alloc_aligned(TF_Arena *arena, usize size, usize alignment) {
    if (!arena || !arena->memory) {
        TF_ERROR("Invalid arena");
        return TF_NULL;
    }

    if (size == 0) {
        return TF_NULL;
    }

    if (!tf_memory_is_power_of_two(alignment)) {
        TF_ERROR("Alignment must be power of two");
        return TF_NULL;
    }

    // Calculate aligned offset
    usize aligned_offset = tf_memory_align_size(arena->offset, alignment);
    usize new_offset = aligned_offset + size;

    if (new_offset > arena->size) {
        TF_ERROR("Arena out of memory (requested %llu bytes, available %llu bytes)",
                 (unsigned long long)size,
                 (unsigned long long)(arena->size - aligned_offset));
        return TF_NULL;
    }

    void *ptr = arena->memory + aligned_offset;
    arena->offset = new_offset;

    // Update peak usage
    if (arena->offset > arena->peak_usage) {
        arena->peak_usage = arena->offset;
    }

    return ptr;
}

TF_API void tf_arena_clear(TF_Arena *arena) {
    if (!arena) {
        TF_WARN("Attempted to clear null arena");
        return;
    }

    TF_DEBUG_TRACE("Clearing arena '%s'", arena->base.name ? arena->base.name : "unnamed");
    arena->offset = 0;
}

TF_API usize tf_arena_get_usage(TF_Arena *arena) {
    return arena ? arena->offset : 0;
}

TF_API usize tf_arena_get_peak_usage(TF_Arena *arena) {
    return arena ? arena->peak_usage : 0;
}

// Pool allocator implementation
TF_API TF_Pool *tf_pool_create(const char *name, usize block_size, usize block_count) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return TF_NULL;
    }

    if (block_size == 0 || block_count == 0) {
        TF_ERROR("Pool block size and count must be greater than zero");
        return TF_NULL;
    }

    // Align block size
    block_size = tf_memory_align_size(block_size, TF_MEMORY_ALIGNMENT);
    usize total_size = block_size * block_count;

    TF_DEBUG("Creating pool '%s' with %llu blocks of %llu bytes each",
             name ? name : "unnamed", (unsigned long long)block_count, (unsigned long long)block_size);

    // Allocate pool structure
    TF_Pool *pool = (TF_Pool *) malloc(sizeof(TF_Pool));
    if (!pool) {
        TF_ERROR("Failed to allocate pool structure");
        return TF_NULL;
    }

    // Allocate pool memory
    pool->memory = (u8 *) malloc(total_size);
    if (!pool->memory) {
        TF_ERROR("Failed to allocate pool memory");
        free(pool);
        return TF_NULL;
    }

    // Allocate free list
    pool->free_list = (void **) malloc(sizeof(void *) * block_count);
    if (!pool->free_list) {
        TF_ERROR("Failed to allocate pool free list");
        free(pool->memory);
        free(pool);
        return TF_NULL;
    }

    // Initialize pool
    pool->base.type = TF_ALLOCATOR_TYPE_POOL;
    pool->base.alloc = tf_pool_alloc_func;
    pool->base.free = tf_pool_free_func;
    pool->base.clear = tf_pool_clear_func;
    pool->base.data = pool;
    pool->base.name = name;
    pool->total_size = total_size;
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->free_count = block_count;
    pool->peak_usage = 0;

    // Initialize free list
    for (usize i = 0; i < block_count; i++) {
        pool->free_list[i] = pool->memory + (i * block_size);
    }

    s_memory_state.stats.pool_count++;

    TF_INFO("Pool '%s' created successfully", name ? name : "unnamed");
    return pool;
}

TF_API void tf_pool_destroy(TF_Pool *pool) {
    if (!pool) {
        TF_WARN("Attempted to destroy null pool");
        return;
    }

    TF_DEBUG("Destroying pool '%s'", pool->base.name ? pool->base.name : "unnamed");

    if (pool->memory) {
        free(pool->memory);
    }

    if (pool->free_list) {
        free(pool->free_list);
    }

    free(pool);

    s_memory_state.stats.pool_count--;
    TF_INFO("Pool destroyed");
}

TF_API void *tf_pool_alloc(TF_Pool *pool) {
    if (!pool || !pool->memory || !pool->free_list) {
        TF_ERROR("Invalid pool");
        return TF_NULL;
    }

    if (pool->free_count == 0) {
        TF_ERROR("Pool out of blocks");
        return TF_NULL;
    }

    // Get block from free list
    pool->free_count--;
    void *ptr = pool->free_list[pool->free_count];

    // Update peak usage
    usize current_usage = (pool->block_count - pool->free_count) * pool->block_size;
    if (current_usage > pool->peak_usage) {
        pool->peak_usage = current_usage;
    }

    return ptr;
}

TF_API void tf_pool_free(TF_Pool *pool, void *ptr) {
    if (!pool || !ptr) {
        return;
    }

    // Verify pointer is within pool bounds
    u8 *byte_ptr = (u8 *) ptr;
    if (byte_ptr < pool->memory || byte_ptr >= pool->memory + pool->total_size) {
        TF_ERROR("Pointer not within pool bounds");
        return;
    }

    // Verify pointer is properly aligned
    usize offset = byte_ptr - pool->memory;
    if (offset % pool->block_size != 0) {
        TF_ERROR("Pointer not aligned to block boundary");
        return;
    }

    if (pool->free_count >= pool->block_count) {
        TF_ERROR("Pool free list overflow - double free detected");
        return;
    }

    // Add to free list
    pool->free_list[pool->free_count] = ptr;
    pool->free_count++;
}

TF_API usize tf_pool_get_usage(TF_Pool *pool) {
    if (!pool) {
        return 0;
    }
    return (pool->block_count - pool->free_count) * pool->block_size;
}

TF_API usize tf_pool_get_free_count(TF_Pool *pool) {
    return pool ? pool->free_count : 0;
}

// Stack allocator implementation
TF_API TF_Stack *tf_stack_create(const char *name, usize size) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return TF_NULL;
    }

    if (size == 0) {
        TF_ERROR("Stack size cannot be zero");
        return TF_NULL;
    }

    TF_DEBUG("Creating stack '%s' with size %llu bytes",
             name ? name : "unnamed", (unsigned long long)size);

    // Allocate stack structure
    TF_Stack *stack = (TF_Stack *) malloc(sizeof(TF_Stack));
    if (!stack) {
        TF_ERROR("Failed to allocate stack structure");
        return TF_NULL;
    }

    // Allocate stack memory
    stack->memory = (u8 *) malloc(size);
    if (!stack->memory) {
        TF_ERROR("Failed to allocate stack memory");
        free(stack);
        return TF_NULL;
    }

    // Initialize stack
    stack->base.type = TF_ALLOCATOR_TYPE_STACK;
    stack->base.alloc = tf_stack_alloc_func;
    stack->base.free = tf_stack_free_func;
    stack->base.clear = tf_stack_clear_func;
    stack->base.data = stack;
    stack->base.name = name;
    stack->size = size;
    stack->offset = 0;
    stack->peak_usage = 0;
    stack->marker_count = 0;

    s_memory_state.stats.stack_count++;

    TF_INFO("Stack '%s' created successfully", name ? name : "unnamed");
    return stack;
}

TF_API void tf_stack_destroy(TF_Stack *stack) {
    if (!stack) {
        TF_WARN("Attempted to destroy null stack");
        return;
    }

    TF_DEBUG("Destroying stack '%s'", stack->base.name ? stack->base.name : "unnamed");

    if (stack->memory) {
        free(stack->memory);
    }

    free(stack);

    s_memory_state.stats.stack_count--;
    TF_INFO("Stack destroyed");
}

TF_API void *tf_stack_alloc(TF_Stack *stack, usize size) {
    return tf_stack_alloc_aligned(stack, size, TF_MEMORY_ALIGNMENT);
}

TF_API void *tf_stack_alloc_aligned(TF_Stack *stack, usize size, usize alignment) {
    if (!stack || !stack->memory) {
        TF_ERROR("Invalid stack");
        return TF_NULL;
    }

    if (size == 0) {
        return TF_NULL;
    }

    if (!tf_memory_is_power_of_two(alignment)) {
        TF_ERROR("Alignment must be power of two");
        return TF_NULL;
    }

    // Calculate aligned offset
    usize aligned_offset = tf_memory_align_size(stack->offset, alignment);
    usize new_offset = aligned_offset + size;

    if (new_offset > stack->size) {
        TF_ERROR("Stack out of memory (requested %llu bytes, available %llu bytes)",
                 (unsigned long long)size,
                 (unsigned long long)(stack->size - aligned_offset));
        return TF_NULL;
    }

    void *ptr = stack->memory + aligned_offset;
    stack->offset = new_offset;

    // Update peak usage
    if (stack->offset > stack->peak_usage) {
        stack->peak_usage = stack->offset;
    }

    return ptr;
}

TF_API void tf_stack_push_marker(TF_Stack *stack) {
    if (!stack) {
        TF_WARN("Attempted to push marker on null stack");
        return;
    }

    if (stack->marker_count >= TF_ARRAY_COUNT(stack->markers)) {
        TF_ERROR("Stack marker overflow - too many nested scopes");
        return;
    }

    stack->markers[stack->marker_count] = stack->offset;
    stack->marker_count++;

    TF_DEBUG_TRACE("Stack marker pushed at offset %llu", (unsigned long long)stack->offset);
}

TF_API void tf_stack_pop_marker(TF_Stack *stack) {
    if (!stack) {
        TF_WARN("Attempted to pop marker from null stack");
        return;
    }

    if (stack->marker_count == 0) {
        TF_ERROR("Stack marker underflow - no markers to pop");
        return;
    }

    stack->marker_count--;
    usize marker_offset = stack->markers[stack->marker_count];
    stack->offset = marker_offset;

    TF_DEBUG_TRACE("Stack marker popped to offset %llu", (unsigned long long)marker_offset);
}

TF_API void tf_stack_clear(TF_Stack *stack) {
    if (!stack) {
        TF_WARN("Attempted to clear null stack");
        return;
    }

    TF_DEBUG("Clearing stack '%s'", stack->base.name ? stack->base.name : "unnamed");
    stack->offset = 0;
    stack->marker_count = 0;
}

TF_API usize tf_stack_get_usage(TF_Stack *stack) {
    return stack ? stack->offset : 0;
}

// Debug memory tracking (only in debug builds)
#if TF_MEMORY_TRACK_ALLOCATIONS

TF_API void tf_memory_debug_print_allocations(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return;
    }

    TF_INFO("Active allocations (%u):", s_memory_state.allocation_count);
    for (u32 i = 0; i < s_memory_state.allocation_count; i++) {
        TF_INFO("  [%u] %p - %llu bytes at %s:%u",
                i,
                s_memory_state.allocations[i].ptr,
                (unsigned long long)s_memory_state.allocations[i].size,
                s_memory_state.allocations[i].file ? s_memory_state.allocations[i].file : "unknown",
                s_memory_state.allocations[i].line);
    }
}

TF_API void tf_memory_debug_check_leaks(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return;
    }

    if (s_memory_state.allocation_count > 0) {
        TF_ERROR("Memory leaks detected: %u allocations not freed", s_memory_state.allocation_count);
        tf_memory_debug_print_allocations();
    } else {
        TF_INFO("No memory leaks detected");
    }
}

#endif // TF_MEMORY_TRACK_ALLOCATIONS
