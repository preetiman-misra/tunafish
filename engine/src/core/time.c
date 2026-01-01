//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/time.h"
#include "tunafish/core/log.h"
#include "tunafish/core/export.h"

// Platform-specific includes
#ifdef TF_PLATFORM_WINDOWS
    #include <windows.h>
#elif defined(TF_PLATFORM_MACOS) || defined(TF_PLATFORM_LINUX)
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#endif

// Time state
static struct {
    b32 initialized;
    f64 start_time;
    f64 last_frame_time;
    f32 delta_time;
    f64 elapsed_time;
    u64 frame_count;
    // FPS tracking (using running sum for O(1) calculation)
    f32 fps_history[60];
    u32 fps_index;
    f32 fps_sum;        // Running sum of fps_history values
    u32 fps_count;      // Number of valid entries (0-60 during warmup)
    f32 current_fps;
    // Frame rate control
    f32 target_fps;
    f64 target_frame_time;
    f64 last_target_check;
} s_time_state = {0};

// Platform-specific time functions
#ifdef TF_PLATFORM_WINDOWS

static f64 tf_time_get_frequency(void) {
    static f64 frequency = 0.0;
    if (frequency == 0.0) {
        LARGE_INTEGER freq;
        if (!QueryPerformanceFrequency(&freq)) {
            TF_ERROR("QueryPerformanceFrequency failed");
            return 1.0; // Fallback
        }
        frequency = (f64)freq.QuadPart;
    }
    return frequency;
}

static f64 tf_time_get_current_platform(void) {
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        TF_ERROR("QueryPerformanceCounter failed");
        return 0.0;
    }
    return (f64)counter.QuadPart / tf_time_get_frequency();
}

static void tf_time_sleep_platform(f32 seconds) {
    if (seconds > 0.0f) {
        Sleep((u32)(seconds * 1000.0f + 0.5f));  // Use u32 for consistency
    }
}

#else // POSIX (macOS, Linux)

static f64 tf_time_get_current_platform(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        TF_ERROR("clock_gettime failed");
        return 0.0;
    }
    return (f64) ts.tv_sec + (f64) ts.tv_nsec / 1000000000.0;
}

static void tf_time_sleep_platform(f32 seconds) {
    if (seconds > 0.0f) {
        struct timespec ts;
        ts.tv_sec = (time_t) seconds;
        ts.tv_nsec = (long) ((seconds - (f32) ts.tv_sec) * 1000000000.0f);
        nanosleep(&ts, TF_NULL); // Use TF_NULL for consistency
    }
}

#endif


// Public API implementation
TF_API void tf_time_init(void) {
    if (s_time_state.initialized) {
        TF_WARN("Time system already initialized");
        return;
    }
    TF_DEBUG("Initializing time system...");
    // Initialize state
    s_time_state.start_time = tf_time_get_current_platform();
    s_time_state.last_frame_time = s_time_state.start_time;
    s_time_state.delta_time = 0.0f;
    s_time_state.elapsed_time = 0.0;
    s_time_state.frame_count = 0;
    s_time_state.fps_index = 0;
    s_time_state.fps_sum = 0.0f;
    s_time_state.fps_count = 0;
    s_time_state.current_fps = 0.0f;
    s_time_state.target_fps = 0.0f; // Unlimited by default
    s_time_state.target_frame_time = 0.0;
    s_time_state.last_target_check = s_time_state.start_time;
    // Clear FPS history
    for (u32 i = 0; i < 60; i++) {
        s_time_state.fps_history[i] = 0.0f;
    }
    s_time_state.initialized = TF_TRUE;
    TF_INFO("Time system initialized");
}

TF_API void tf_time_shutdown(void) {
    if (!s_time_state.initialized) {
        TF_WARN("Time system not initialized");
        return;
    }
    TF_DEBUG("Shutting down time system...");
    s_time_state.initialized = TF_FALSE;
    TF_INFO("Time system shutdown");
}

TF_API void tf_time_update(void) {
    if (!s_time_state.initialized) {
        TF_WARN("Time system not initialized");
        return;
    }
    f64 current_time = tf_time_get_current_platform();
    s_time_state.delta_time = (f32) (current_time - s_time_state.last_frame_time);
    s_time_state.elapsed_time = current_time - s_time_state.start_time;
    s_time_state.last_frame_time = current_time;
    s_time_state.frame_count++;
    // Clamp delta time to reasonable values (prevent huge jumps)
    if (s_time_state.delta_time > 0.1f) {
        s_time_state.delta_time = 0.1f; // Cap at 100ms
    }
    // Update FPS using O(1) running sum
    if (s_time_state.delta_time > 0.0001f) {
        f32 instant_fps = 1.0f / s_time_state.delta_time;
        if (instant_fps > 10000.0f) {
            instant_fps = 10000.0f;
        }
        // Subtract old value from sum, add new value
        s_time_state.fps_sum -= s_time_state.fps_history[s_time_state.fps_index];
        s_time_state.fps_history[s_time_state.fps_index] = instant_fps;
        s_time_state.fps_sum += instant_fps;
        // Track count during warmup period
        if (s_time_state.fps_count < 60) {
            s_time_state.fps_count++;
        }
        s_time_state.fps_index = (s_time_state.fps_index + 1) % 60;
        s_time_state.current_fps = s_time_state.fps_count > 0
            ? s_time_state.fps_sum / (f32)s_time_state.fps_count
            : 0.0f;
    }
}

TF_API f32 tf_time_get_delta(void) {
    return s_time_state.initialized ? s_time_state.delta_time : 0.0f;
}

TF_API f64 tf_time_get_elapsed(void) {
    return s_time_state.initialized ? s_time_state.elapsed_time : 0.0;
}

TF_API u64 tf_time_get_frame_count(void) {
    return s_time_state.initialized ? s_time_state.frame_count : 0;
}

TF_API f32 tf_time_get_fps(void) {
    return s_time_state.initialized ? s_time_state.current_fps : 0.0f;
}

TF_API f64 tf_time_get_current(void) {
    return tf_time_get_current_platform();
}

TF_API void tf_time_sleep(f32 seconds) {
    if (seconds > 0.0f) {
        tf_time_sleep_platform(seconds);
    }
}

TF_API u64 tf_time_get_ticks(void) {
#ifdef TF_PLATFORM_WINDOWS
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        TF_ERROR("QueryPerformanceCounter failed");
        return 0;
    }
    return (u64)counter.QuadPart;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        TF_ERROR("clock_gettime failed");
        return 0;
    }
    return (u64) ts.tv_sec * 1000000000ULL + (u64) ts.tv_nsec;
#endif
}

TF_API void tf_time_set_target_fps(f32 fps) {
    if (!s_time_state.initialized) {
        TF_WARN("Time system not initialized");
        return;
    }
    s_time_state.target_fps = fps;
    s_time_state.target_frame_time = fps > 0.0f ? 1.0 / (f64) fps : 0.0;
    s_time_state.last_target_check = tf_time_get_current_platform();
    if (fps > 0.0f) {
        TF_DEBUG("Target FPS set to %.1f (%.3fms per frame)", fps, s_time_state.target_frame_time * 1000.0);
    } else {
        TF_DEBUG("Target FPS set to unlimited");
    }
}

TF_API f32 tf_time_get_target_fps(void) {
    return s_time_state.initialized ? s_time_state.target_fps : 0.0f;
}

TF_API b32 tf_time_should_render(void) {
    if (!s_time_state.initialized) {
        return TF_TRUE;
    }
    // If no target FPS, always render
    if (s_time_state.target_fps <= 0.0f) {
        return TF_TRUE;
    }
    // Check if enough time has passed for target FPS
    f64 current_time = tf_time_get_current_platform();
    f64 time_since_last = current_time - s_time_state.last_target_check;

    if (time_since_last >= s_time_state.target_frame_time) {
        s_time_state.last_target_check = current_time;
        return TF_TRUE;
    }

    return TF_FALSE;
}
