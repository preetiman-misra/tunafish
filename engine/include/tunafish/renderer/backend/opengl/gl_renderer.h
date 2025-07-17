//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/renderer/backend/renderer_backend.h"
#include "tunafish/core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// OpenGL-specific data
typedef struct {
    u32 vao; // Default vertex array object
    u32 vbo; // Default vertex buffer
    u32 shader; // Current shader program

    // Viewport state
    i32 viewport_x, viewport_y;
    u32 viewport_width, viewport_height;

    // Clear state
    TF_Color clear_color;
} TF_OpenGLData;

// OpenGL backend creation
TF_RendererBackend *tf_renderer_backend_create_opengl(void);

#ifdef __cplusplus
}
#endif
