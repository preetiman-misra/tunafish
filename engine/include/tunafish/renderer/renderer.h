//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"
#include "tunafish/core/math.h"
#include "tunafish/renderer/renderer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct TF_Renderer TF_Renderer;
typedef struct TF_Window TF_Window;
typedef struct TF_Camera TF_Camera;
typedef struct TF_Mesh TF_Mesh;

// Renderer configuration
typedef struct {
    TF_RendererBackend backend;
    b32 enable_depth_test;
    b32 enable_vsync;
    TF_Color clear_color;
} TF_RendererConfig;

// Core renderer lifecycle
TF_API TF_Renderer *tf_renderer_create(TF_Window *window, const TF_RendererConfig *config);

TF_API void tf_renderer_destroy(TF_Renderer *renderer);

// Frame management
TF_API void tf_renderer_begin_frame(TF_Renderer *renderer);

TF_API void tf_renderer_end_frame(TF_Renderer *renderer);

TF_API void tf_renderer_clear(TF_Renderer *renderer, TF_ClearFlags flags);

// Camera management
TF_API void tf_renderer_set_camera(TF_Renderer *renderer, TF_Camera *camera);

// Basic drawing
TF_API void tf_renderer_draw_triangle(TF_Renderer *renderer, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color);

TF_API void tf_renderer_draw_mesh(TF_Renderer *renderer, TF_Mesh *mesh, TF_Mat4 transform);

#ifdef __cplusplus
}
#endif
