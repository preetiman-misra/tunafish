//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplucplus
extern "C" {
#endif

// Window handle
typedef struct TF_Window TF_Window;

// Window configuration
typedef struct {
    const char *title;
    u32 width;
    u32 height;
    b32 resizable;
    b32 fullscreen;
} TF_WindowConfig;

// Window API
TF_API TF_Window *tf_window_create(const TF_WindowConfig *config);

TF_API void tf_window_destroy(TF_Window *window);

TF_API b32 tf_window_should_close(TF_Window *window);

TF_API void tf_window_poll_events(TF_Window *window);

TF_API void tf_window_swap_buffers(TF_Window *window);

TF_API void tf_window_get_size(TF_Window *window, u32 *width, u32 *height);

TF_API void tf_window_set_title(TF_Window *window, const char *title);

#ifdef __cplucplus
}
#endif
