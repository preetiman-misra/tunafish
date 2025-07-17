//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/tunafish.h"
#include <stdlib.h>

// Engine structure (implementation details)
struct TF_Engine {
    b32 initialized;
    b32 running;
};

TF_API TF_Engine *tf_engine_create(void) {
    TF_DEBUG("Creating engine...");
    TF_Engine *engine = (TF_Engine *) malloc(sizeof(TF_Engine));
    if (!engine) {
        TF_ERROR("Failed to allocate memory for engine");
        return TF_NULL;
    }
    engine->initialized = TF_FALSE;
    engine->running = TF_FALSE;
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
    // Initialize logging system
    tf_log_init(TF_LOG_LEVEL_DEBUG, TF_LOG_OUTPUT_CONSOLE, TF_NULL);
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

    // Shutdown logging last
    tf_log_shutdown();
}

TF_API b32 tf_engine_is_running(const TF_Engine *engine) {
    return engine ? engine->running : TF_FALSE;
}

TF_API void tf_engine_run_frame(TF_Engine *engine) {
    if (!engine || !engine->running) {
        return;
    }

    // TODO: Frame logic will go here
    TF_DEBUG_TRACE("Running frame");
}
