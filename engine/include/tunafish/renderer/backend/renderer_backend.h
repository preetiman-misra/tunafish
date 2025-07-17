//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/renderer/renderer_types.h"
#include "tunafish/core/math.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct TF_RendererBackend TF_RendererBackend;
typedef struct TF_Window TF_Window;
typedef struct TF_Camera TF_Camera;
typedef struct TF_Mesh TF_Mesh;

// Backend function pointers (vtable)
typedef struct {
    // Lifecycle
    b32 (*create)(TF_RendererBackend *backend, TF_Window *window);

    void (*destroy)(TF_RendererBackend *backend);

    // Frame management
    void (*begin_frame)(TF_RendererBackend *backend);

    void (*end_frame)(TF_RendererBackend *backend);

    void (*clear)(TF_RendererBackend *backend, TF_ClearFlags flags);

    // State management
    void (*set_clear_color)(TF_RendererBackend *backend, TF_Color color);

    void (*set_viewport)(TF_RendererBackend *backend, i32 x, i32 y, u32 width, u32 height);

    // Drawing
    void (*draw_triangle)(TF_RendererBackend *backend, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color);
} TF_RendererBackendVTable;

// Backend base structure
struct TF_RendererBackend {
    TF_RendererBackendVTable *vtable;
    void *data; // Backend-specific data
};

// Backend creation functions
TF_RendererBackend *tf_renderer_backend_create_opengl(void);

#ifdef __cplusplus
}
#endif
