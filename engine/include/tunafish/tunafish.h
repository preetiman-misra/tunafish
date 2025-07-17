//
// Created by Preetiman Misra on 17/07/25.
//

#pragma once

// Core engine includes
#include "tunafish/core/types.h"
#include "tunafish/core/export.h"
#include "tunafish/platform/window.h"

#ifdef __cplusplus
extern "C" {
#endif

// Engine handle (opaque)
typedef struct TF_Engine TF_Engine;

// Basic engine lifecycle
TF_API TF_Engine *tf_engine_create(void);

TF_API void tf_engine_destroy(TF_Engine *engine);

TF_API b32 tf_engine_initialize(TF_Engine *engine);

TF_API void tf_engine_shutdown(TF_Engine *engine);

TF_API b32 tf_engine_is_running(const TF_Engine *engine);

TF_API void tf_engine_run_frame(TF_Engine *engine);

#ifdef __cplusplus
}
#endif
