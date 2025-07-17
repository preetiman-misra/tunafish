//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/tunafish.h"
#include <stdlib.h>

// Engine structure (implementation details)
struct TF_Engine {
    b32 initialized;
    b32 running;
    // Core allocators for different engine subsystems
    TF_Arena *general_arena; // For general engine allocations
    TF_Arena *frame_arena; // For per-frame allocations (cleared each frame)
    TF_Pool *entity_pool; // For game entities (future ECS)
    TF_Stack *temp_stack; // For temporary calculations
};

// Create engine-specific allocators
static b32 tf_engine_create_allocators(TF_Engine *engine) {
    TF_DEBUG("Creating engine allocators...");
    // General arena for long-lived engine data (1 MB)
    engine->general_arena = tf_arena_create("engine_general", TF_MEGABYTES(1));
    if (!engine->general_arena) {
        TF_ERROR("Failed to create general arena");
        return TF_FALSE;
    }
    // Frame arena for temporary per-frame data (512 KB, cleared each frame)
    engine->frame_arena = tf_arena_create("engine_frame", TF_KILOBYTES(512));
    if (!engine->frame_arena) {
        TF_ERROR("Failed to create frame arena");
        return TF_FALSE;
    }
    // Entity pool for game objects (1000 entities of 256 bytes each)
    engine->entity_pool = tf_pool_create("engine_entities", 256, 1000);
    if (!engine->entity_pool) {
        TF_ERROR("Failed to create entity pool");
        return TF_FALSE;
    }
    // Temp stack for calculations (256 KB with marker support)
    engine->temp_stack = tf_stack_create("engine_temp", TF_KILOBYTES(256));
    if (!engine->temp_stack) {
        TF_ERROR("Failed to create temp stack");
        return TF_FALSE;
    }
    TF_INFO("Engine allocators created successfully");
    return TF_TRUE;
}

TF_API TF_Engine *tf_engine_create(void) {
    TF_DEBUG("Creating engine...");
    // Use system malloc for engine structure itself (before memory system is initialized)
    TF_Engine *engine = (TF_Engine *) malloc(sizeof(TF_Engine));
    if (!engine) {
        TF_ERROR("Failed to allocate memory for engine");
        return TF_NULL;
    }
    // Initialize basic state
    engine->initialized = TF_FALSE;
    engine->running = TF_FALSE;
    engine->general_arena = TF_NULL;
    engine->frame_arena = TF_NULL;
    engine->entity_pool = TF_NULL;
    engine->temp_stack = TF_NULL;
    TF_INFO("Engine created successfully");
    return engine;
}

TF_API void tf_engine_destroy(TF_Engine *engine) {
    if (!engine) {
        TF_WARN("Attempted to destroy null engine");
        return;
    }
    TF_DEBUG("Destroying engine...");
    if (engine->initialized) {
        tf_engine_shutdown(engine);
    }
    // Clean up allocators if they exist
    if (engine->temp_stack) {
        tf_stack_destroy(engine->temp_stack);
    }
    if (engine->entity_pool) {
        tf_pool_destroy(engine->entity_pool);
    }
    if (engine->frame_arena) {
        tf_arena_destroy(engine->frame_arena);
    }
    if (engine->general_arena) {
        tf_arena_destroy(engine->general_arena);
    }
    // Free engine structure itself
    free(engine);
    TF_INFO("Engine destroyed");
}

TF_API b32 tf_engine_initialize(TF_Engine *engine) {
    if (!engine) {
        TF_ERROR("Cannot initialize null engine");
        return TF_FALSE;
    }
    if (engine->initialized) {
        TF_WARN("Engine already initialized");
        return TF_FALSE;
    }
    TF_INFO("Initializing engine...");
    // Initialize systems in order:
    // 1. Logging system (first, so we can log everything else)
    tf_log_init(TF_LOG_LEVEL_DEBUG, TF_LOG_OUTPUT_CONSOLE, TF_NULL);
    // 2. Memory system (second, so other systems can use custom allocators)
    tf_memory_init();
    // 3. Time system
    tf_time_init();
    // 4. Create engine allocators
    if (!tf_engine_create_allocators(engine)) {
        tf_time_shutdown();
        tf_memory_shutdown();
        tf_log_shutdown();
        return TF_FALSE;
    }
    engine->initialized = TF_TRUE;
    engine->running = TF_TRUE;
    TF_INFO("Engine initialized successfully");
    return TF_TRUE;
}

TF_API void tf_engine_shutdown(TF_Engine *engine) {
    if (!engine) {
        TF_WARN("Attempted to shutdown null engine");
        return;
    }
    if (!engine->initialized) {
        TF_WARN("Engine not initialized");
        return;
    }
    TF_INFO("Shutting down engine...");
    engine->running = TF_FALSE;
    engine->initialized = TF_FALSE;
    // Shutdown in reverse order:
    // 1. Time system
    tf_time_shutdown();
    // 2. Memory system (second to last, so we can still track cleanup)
    tf_memory_shutdown();

    // 3. Logging system (last, so we can log everything)
    tf_log_shutdown();
}

TF_API b32 tf_engine_is_running(const TF_Engine *engine) {
    return engine ? engine->running : TF_FALSE;
}

TF_API void tf_engine_run_frame(TF_Engine *engine) {
    if (!engine || !engine->running) {
        return;
    }

    // Update time system
    tf_time_update();

    // Clear per-frame arena for this frame's temporary allocations
    if (engine->frame_arena) {
        tf_arena_clear(engine->frame_arena);
    }

    // TODO: Renderer will use frame_arena for temporary data
    // TODO: Input system will use temp_stack for processing
    // TODO: Game logic will use entity_pool for objects

    TF_DEBUG_TRACE("Running frame (delta: %.3fms)", tf_time_get_delta() * 1000.0f);
}
