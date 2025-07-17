//
// Created by Preetiman Misra on 17/07/25.
//
#include <tunafish/tunafish.h>
#include <stdlib.h>

struct TF_Engine {
    b32 initialized;
    b32 running;
};

TF_API TF_Engine *tf_engine_create(void) {
    TF_Engine *engine = (TF_Engine *) malloc(sizeof(TF_Engine));
    if (!engine) {
        return TF_NULL;
    }

    engine->initialized = TF_FALSE;
    engine->running = TF_FALSE;

    return engine;
};

TF_API void tf_engine_destroy(TF_Engine *engine) {
    if (!engine) return;

    if (engine->initialized) {
        tf_engine_shutdown(engine);
    }

    free(engine);
}

TF_API b32 tf_engine_initialize(TF_Engine *engine) {
    if (!engine || engine->initialized) {
        return TF_FALSE;
    }

    engine->initialized = TF_TRUE;
    engine->running = TF_FALSE;

    return TF_TRUE;
}

TF_API void tf_engine_shutdown(TF_Engine *engine) {
    if (!engine || !engine->initialized) {
        return;
    }

    engine->running = TF_FALSE;
    engine->initialized = TF_FALSE;
}

TF_API b32 tf_engine_is_running(const TF_Engine *engine) {
    return engine ? engine->running : TF_FALSE;
}

TF_API void tf_engine_run_frame(TF_Engine *engine) {
    if (!engine || !engine->running) {
        return;
    }

    // TODO: Frame logic will go here
    // For now, just a basic stub
}
