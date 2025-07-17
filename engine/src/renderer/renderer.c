//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/renderer/renderer.h"
#include "tunafish/renderer/backend/renderer_backend.h"
#include "tunafish/renderer/backend/opengl/gl_renderer.h"
#include "tunafish/core/log.h"
#include "tunafish/core/memory.h"
#include "tunafish/platform/window.h"
#include <stdlib.h>

// Renderer structure
struct TF_Renderer {
    TF_RendererBackend *backend;
    TF_Camera *current_camera;
    TF_RendererConfig config;
};

TF_Renderer *tf_renderer_create(TF_Window *window, const TF_RendererConfig *config) {
    if (!window || !config) {
        TF_ERROR("Invalid parameters for renderer creation");
        return TF_NULL;
    }

    TF_INFO("Creating renderer with backend type: %d", config->backend);

    // Allocate renderer
    TF_Renderer *renderer = malloc(sizeof(TF_Renderer));
    if (!renderer) {
        TF_ERROR("Failed to allocate renderer");
        return TF_NULL;
    }

    // Store config
    renderer->config = *config;
    renderer->current_camera = TF_NULL;

    // Create backend based on type
    switch (config->backend) {
        case TF_RENDERER_BACKEND_OPENGL:
            renderer->backend = tf_renderer_backend_create_opengl();
            break;
        case TF_RENDERER_BACKEND_VULKAN:
            TF_ERROR("Vulkan backend not implemented yet");
            free(renderer);
            return TF_NULL;
        default:
            TF_ERROR("Unknown renderer backend type: %d", config->backend);
            free(renderer);
            return TF_NULL;
    }

    if (!renderer->backend) {
        TF_ERROR("Failed to create renderer backend");
        free(renderer);
        return TF_NULL;
    }

    // Initialize backend
    if (!renderer->backend->vtable->create(renderer->backend, window)) {
        TF_ERROR("Failed to initialize renderer backend");
        free(renderer->backend);
        free(renderer);
        return TF_NULL;
    }

    // Apply initial configuration
    renderer->backend->vtable->set_clear_color(renderer->backend, config->clear_color);

    TF_INFO("Renderer created successfully");
    return renderer;
}

void tf_renderer_destroy(TF_Renderer *renderer) {
    if (!renderer) {
        return;
    }

    TF_DEBUG("Destroying renderer...");

    if (renderer->backend) {
        renderer->backend->vtable->destroy(renderer->backend);
        free(renderer->backend);
    }

    free(renderer);
    TF_INFO("Renderer destroyed");
}

void tf_renderer_begin_frame(TF_Renderer *renderer) {
    if (!renderer || !renderer->backend) {
        return;
    }

    renderer->backend->vtable->begin_frame(renderer->backend);
}

void tf_renderer_end_frame(TF_Renderer *renderer) {
    if (!renderer || !renderer->backend) {
        return;
    }

    renderer->backend->vtable->end_frame(renderer->backend);
}

void tf_renderer_clear(TF_Renderer *renderer, TF_ClearFlags flags) {
    if (!renderer || !renderer->backend) {
        return;
    }

    renderer->backend->vtable->clear(renderer->backend, flags);
}

void tf_renderer_set_camera(TF_Renderer *renderer, TF_Camera *camera) {
    if (!renderer) {
        return;
    }

    renderer->current_camera = camera;
}

void tf_renderer_draw_triangle(TF_Renderer *renderer, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color) {
    if (!renderer || !renderer->backend) {
        return;
    }

    renderer->backend->vtable->draw_triangle(renderer->backend, p1, p2, p3, color);
}

void tf_renderer_draw_mesh(TF_Renderer *renderer, TF_Mesh *mesh, TF_Mat4 transform) {
    if (!renderer || !renderer->backend || !mesh) {
        return;
    }

    // For now, just log that we're drawing a mesh
    // We'll implement actual mesh rendering later
    TF_DEBUG_TRACE("Drawing mesh with transform");
}
