//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/memory.h"
#include "tunafish/core/log.h"
#include <stdlib.h>
#include <string.h>

// =============================================================================
// Internal tracking allocator (for allocator backing memory)
// =============================================================================

// Header stored before each tracked allocation
typedef struct {
    usize size;
} TF_AllocHeader;

// Global memory state
static struct {
    b32 initialized;
    TF_MemoryStats stats;
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

// Internal tracked allocation (for allocator backing memory)
static void *tf_tracked_alloc(usize size) {
    if (size == 0) return NULL;

    usize total = sizeof(TF_AllocHeader) + size;
    TF_AllocHeader *header = (TF_AllocHeader *)malloc(total);
    if (!header) return NULL;

    header->size = size;
    s_memory_state.stats.total_allocated += size;
    s_memory_state.stats.current_allocated += size;
    s_memory_state.stats.allocation_count++;

    if (s_memory_state.stats.current_allocated > s_memory_state.stats.peak_allocated) {
        s_memory_state.stats.peak_allocated = s_memory_state.stats.current_allocated;
    }

    return (void *)(header + 1);
}

static void tf_tracked_free(void *ptr) {
    if (!ptr) return;

    TF_AllocHeader *header = ((TF_AllocHeader *)ptr) - 1;
    s_memory_state.stats.current_allocated -= header->size;
    s_memory_state.stats.free_count++;
    free(header);
}

// Alignment helper
static usize tf_align_size(usize size, usize alignment) {
    return TF_ALIGN_UP(size, alignment);
}

// =============================================================================
// Core memory system
// =============================================================================

TF_API void tf_memory_init(void) {
    if (s_memory_state.initialized) {
        TF_WARN("Memory system already initialized");
        return;
    }

    TF_DEBUG("Initializing memory system...");
    memset(&s_memory_state.stats, 0, sizeof(s_memory_state.stats));
    s_memory_state.initialized = TF_TRUE;
    TF_INFO("Memory system initialized");
}

TF_API void tf_memory_shutdown(void) {
    if (!s_memory_state.initialized) {
        TF_WARN("Memory system not initialized");
        return;
    }

    TF_DEBUG("Shutting down memory system...");
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

// =============================================================================
// Arena allocator
// =============================================================================

TF_API TF_Arena *tf_arena_create(const char *name, usize size) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return NULL;
    }

    if (size == 0) {
        TF_ERROR("Arena size cannot be zero");
        return NULL;
    }

#if TF_IS_DEBUG
    TF_DEBUG("Creating arena '%s' with size %llu bytes", name ? name : "unnamed", (unsigned long long)size);
#endif

    TF_Arena *arena = (TF_Arena *)tf_tracked_alloc(sizeof(TF_Arena));
    if (!arena) {
        TF_ERROR("Failed to allocate arena structure");
        return NULL;
    }

    arena->memory = (u8 *)tf_tracked_alloc(size);
    if (!arena->memory) {
        TF_ERROR("Failed to allocate arena memory");
        tf_tracked_free(arena);
        return NULL;
    }

    arena->size = size;
    arena->offset = 0;
    arena->peak_usage = 0;
#if TF_IS_DEBUG
    arena->name = name;
#endif

    s_memory_state.stats.arena_count++;

#if TF_IS_DEBUG
    TF_INFO("Arena '%s' created successfully", name ? name : "unnamed");
#endif
    return arena;
}

TF_API void tf_arena_destroy(TF_Arena *arena) {
    if (!arena) {
        TF_WARN("Attempted to destroy null arena");
        return;
    }

#if TF_IS_DEBUG
    TF_DEBUG("Destroying arena '%s'", arena->name ? arena->name : "unnamed");
#endif

    if (arena->memory) {
        tf_tracked_free(arena->memory);
    }
    tf_tracked_free(arena);

    s_memory_state.stats.arena_count--;
    TF_INFO("Arena destroyed");
}

TF_API void *tf_arena_alloc(TF_Arena *arena, usize size) {
    return tf_arena_alloc_aligned(arena, size, TF_MEMORY_ALIGNMENT);
}

TF_API void *tf_arena_alloc_aligned(TF_Arena *arena, usize size, usize alignment) {
    if (!arena || !arena->memory || size == 0) {
        return NULL;
    }

    usize aligned_offset = tf_align_size(arena->offset, alignment);
    usize new_offset = aligned_offset + size;

    if (new_offset > arena->size) {
        TF_ERROR("Arena out of memory");
        return NULL;
    }

    void *ptr = arena->memory + aligned_offset;
    arena->offset = new_offset;

    if (arena->offset > arena->peak_usage) {
        arena->peak_usage = arena->offset;
    }

    return ptr;
}

TF_API void tf_arena_clear(TF_Arena *arena) {
    if (arena) {
        arena->offset = 0;
    }
}

TF_API usize tf_arena_get_usage(TF_Arena *arena) {
    return arena ? arena->offset : 0;
}

TF_API usize tf_arena_get_peak_usage(TF_Arena *arena) {
    return arena ? arena->peak_usage : 0;
}

// =============================================================================
// Pool allocator (intrusive free list)
// =============================================================================

TF_API TF_Pool *tf_pool_create(const char *name, usize block_size, usize block_count) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return NULL;
    }

    if (block_size == 0 || block_count == 0) {
        TF_ERROR("Pool block size and count must be greater than zero");
        return NULL;
    }

    // Ensure block size can hold a pointer (for intrusive free list)
    if (block_size < sizeof(void *)) {
        block_size = sizeof(void *);
    }
    block_size = tf_align_size(block_size, TF_MEMORY_ALIGNMENT);

#if TF_IS_DEBUG
    TF_DEBUG("Creating pool '%s' with %llu blocks of %llu bytes each",
             name ? name : "unnamed", (unsigned long long)block_count, (unsigned long long)block_size);
#endif

    TF_Pool *pool = (TF_Pool *)tf_tracked_alloc(sizeof(TF_Pool));
    if (!pool) {
        TF_ERROR("Failed to allocate pool structure");
        return NULL;
    }

    usize total_size = block_size * block_count;
    pool->memory = (u8 *)tf_tracked_alloc(total_size);
    if (!pool->memory) {
        TF_ERROR("Failed to allocate pool memory");
        tf_tracked_free(pool);
        return NULL;
    }

    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->free_count = block_count;
    pool->peak_usage = 0;
#if TF_IS_DEBUG
    pool->name = name;
#endif

    // Build intrusive free list (each free block points to the next)
    pool->free_head = pool->memory;
    for (usize i = 0; i < block_count - 1; i++) {
        void **block = (void **)(pool->memory + i * block_size);
        *block = pool->memory + (i + 1) * block_size;
    }
    // Last block points to NULL
    void **last_block = (void **)(pool->memory + (block_count - 1) * block_size);
    *last_block = NULL;

    s_memory_state.stats.pool_count++;

#if TF_IS_DEBUG
    TF_INFO("Pool '%s' created successfully", name ? name : "unnamed");
#endif
    return pool;
}

TF_API void tf_pool_destroy(TF_Pool *pool) {
    if (!pool) {
        TF_WARN("Attempted to destroy null pool");
        return;
    }

#if TF_IS_DEBUG
    TF_DEBUG("Destroying pool '%s'", pool->name ? pool->name : "unnamed");
#endif

    if (pool->memory) {
        tf_tracked_free(pool->memory);
    }
    tf_tracked_free(pool);

    s_memory_state.stats.pool_count--;
    TF_INFO("Pool destroyed");
}

TF_API void *tf_pool_alloc(TF_Pool *pool) {
    if (!pool || !pool->free_head) {
        TF_ERROR("Pool out of blocks");
        return NULL;
    }

    // Pop from free list
    void *block = pool->free_head;
    pool->free_head = *(void **)block;
    pool->free_count--;

    // Track peak usage
    usize current_usage = (pool->block_count - pool->free_count) * pool->block_size;
    if (current_usage > pool->peak_usage) {
        pool->peak_usage = current_usage;
    }

    return block;
}

TF_API void tf_pool_free(TF_Pool *pool, void *ptr) {
    if (!pool || !ptr) return;

    // Verify pointer is within pool bounds
    u8 *byte_ptr = (u8 *)ptr;
    u8 *pool_end = pool->memory + pool->block_size * pool->block_count;
    if (byte_ptr < pool->memory || byte_ptr >= pool_end) {
        TF_ERROR("Pointer not within pool bounds");
        return;
    }

    // Verify alignment
    usize offset = byte_ptr - pool->memory;
    if (offset % pool->block_size != 0) {
        TF_ERROR("Pointer not aligned to block boundary");
        return;
    }

    // Push onto free list
    *(void **)ptr = pool->free_head;
    pool->free_head = ptr;
    pool->free_count++;
}

TF_API usize tf_pool_get_usage(TF_Pool *pool) {
    if (!pool) return 0;
    return (pool->block_count - pool->free_count) * pool->block_size;
}

TF_API usize tf_pool_get_free_count(TF_Pool *pool) {
    return pool ? pool->free_count : 0;
}

// =============================================================================
// Stack allocator
// =============================================================================

TF_API TF_Stack *tf_stack_create(const char *name, usize size) {
    if (!s_memory_state.initialized) {
        TF_ERROR("Memory system not initialized");
        return NULL;
    }

    if (size == 0) {
        TF_ERROR("Stack size cannot be zero");
        return NULL;
    }

#if TF_IS_DEBUG
    TF_DEBUG("Creating stack '%s' with size %llu bytes", name ? name : "unnamed", (unsigned long long)size);
#endif

    TF_Stack *stack = (TF_Stack *)tf_tracked_alloc(sizeof(TF_Stack));
    if (!stack) {
        TF_ERROR("Failed to allocate stack structure");
        return NULL;
    }

    stack->memory = (u8 *)tf_tracked_alloc(size);
    if (!stack->memory) {
        TF_ERROR("Failed to allocate stack memory");
        tf_tracked_free(stack);
        return NULL;
    }

    stack->size = size;
    stack->offset = 0;
    stack->peak_usage = 0;
    stack->marker_count = 0;
#if TF_IS_DEBUG
    stack->name = name;
#endif

    s_memory_state.stats.stack_count++;

#if TF_IS_DEBUG
    TF_INFO("Stack '%s' created successfully", name ? name : "unnamed");
#endif
    return stack;
}

TF_API void tf_stack_destroy(TF_Stack *stack) {
    if (!stack) {
        TF_WARN("Attempted to destroy null stack");
        return;
    }

#if TF_IS_DEBUG
    TF_DEBUG("Destroying stack '%s'", stack->name ? stack->name : "unnamed");
#endif

    if (stack->memory) {
        tf_tracked_free(stack->memory);
    }
    tf_tracked_free(stack);

    s_memory_state.stats.stack_count--;
    TF_INFO("Stack destroyed");
}

TF_API void *tf_stack_alloc(TF_Stack *stack, usize size) {
    return tf_stack_alloc_aligned(stack, size, TF_MEMORY_ALIGNMENT);
}

TF_API void *tf_stack_alloc_aligned(TF_Stack *stack, usize size, usize alignment) {
    if (!stack || !stack->memory || size == 0) {
        return NULL;
    }

    usize aligned_offset = tf_align_size(stack->offset, alignment);
    usize new_offset = aligned_offset + size;

    if (new_offset > stack->size) {
        TF_ERROR("Stack out of memory");
        return NULL;
    }

    void *ptr = stack->memory + aligned_offset;
    stack->offset = new_offset;

    if (stack->offset > stack->peak_usage) {
        stack->peak_usage = stack->offset;
    }

    return ptr;
}

TF_API void tf_stack_push_marker(TF_Stack *stack) {
    if (!stack) return;

    if (stack->marker_count >= 32) {
        TF_ERROR("Stack marker overflow");
        return;
    }

    stack->markers[stack->marker_count++] = stack->offset;
}

TF_API void tf_stack_pop_marker(TF_Stack *stack) {
    if (!stack || stack->marker_count == 0) {
        TF_ERROR("Stack marker underflow");
        return;
    }

    stack->offset = stack->markers[--stack->marker_count];
}

TF_API void tf_stack_clear(TF_Stack *stack) {
    if (stack) {
        stack->offset = 0;
        stack->marker_count = 0;
    }
}

TF_API usize tf_stack_get_usage(TF_Stack *stack) {
    return stack ? stack->offset : 0;
}

// =============================================================================
// Debug utilities
// =============================================================================

#if TF_MEMORY_TRACK_ALLOCATIONS

TF_API void tf_memory_debug_print_allocations(void) {
    if (!s_memory_state.initialized) return;

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
    if (!s_memory_state.initialized) return;

    if (s_memory_state.allocation_count > 0) {
        TF_ERROR("Memory leaks detected: %u allocations not freed", s_memory_state.allocation_count);
        tf_memory_debug_print_allocations();
    } else {
        TF_INFO("No memory leaks detected");
    }
}

#endif
