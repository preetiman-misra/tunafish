//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/renderer/backend/renderer_backend.h"
#include "tunafish/renderer/shader.h"
#include "tunafish/core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// OpenGL-specific data
typedef struct {
    // Triangle rendering
    u32 triangle_vao;
    u32 triangle_vbo;
    TF_Shader *triangle_shader;

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
