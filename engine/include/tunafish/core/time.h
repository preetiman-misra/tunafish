//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

// Time system API
TF_API void tf_time_init(void);

TF_API void tf_time_shutdown(void);

TF_API void tf_time_update(void);

// Time queries
TF_API f32 tf_time_get_delta(void); // Delta time in seconds
TF_API f64 tf_time_get_elapsed(void); // Total elapsed time in seconds
TF_API u64 tf_time_get_frame_count(void); // Total frames rendered
TF_API f32 tf_time_get_fps(void); // Current FPS (averaged)

// Time utilities
TF_API f64 tf_time_get_current(void); // Current time in seconds
TF_API void tf_time_sleep(f32 seconds); // Sleep for given seconds
TF_API u64 tf_time_get_ticks(void); // High-resolution ticks

// Frame rate control
TF_API void tf_time_set_target_fps(f32 fps); // Target FPS (0 = unlimited)
TF_API f32 tf_time_get_target_fps(void); // Get current target FPS
TF_API b32 tf_time_should_render(void); // Frame pacing helper

#ifdef __cplusplus
}
#endif
