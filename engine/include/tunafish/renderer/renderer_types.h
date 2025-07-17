//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/math.h"

#ifdef __cplusplus
extern "C" {
#endif

// Backend types
typedef enum {
    TF_RENDERER_BACKEND_OPENGL,
    TF_RENDERER_BACKEND_VULKAN
} TF_RendererBackendType;

// Clear flags
typedef enum {
    TF_CLEAR_COLOR = 1 << 0,
    TF_CLEAR_DEPTH = 1 << 1,
    TF_CLEAR_ALL = TF_CLEAR_COLOR | TF_CLEAR_DEPTH
} TF_ClearFlags;

// Color representation
typedef struct {
    f32 r, g, b, a;
} TF_Color;

// Color constants
#define TF_COLOR_WHITE ((TF_Color){1.0f, 1.0f, 1.0f, 1.0f})
#define TF_COLOR_RED   ((TF_Color){1.0f, 0.0f, 0.0f, 1.0f})
#define TF_COLOR_GREEN ((TF_Color){0.0f, 1.0f, 0.0f, 1.0f})
#define TF_COLOR_BLUE  ((TF_Color){0.0f, 0.0f, 1.0f, 1.0f})

#ifdef __cplusplus
}
#endif
