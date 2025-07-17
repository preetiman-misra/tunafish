//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/renderer/backend/opengl/gl_renderer.h"
#include "tunafish/core/log.h"
#include "tunafish/core/memory.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>


// OpenGL function implementations
static b32 tf_opengl_create(TF_RendererBackend *backend, TF_Window *window);

static void tf_opengl_destroy(TF_RendererBackend *backend);

static void tf_opengl_begin_frame(TF_RendererBackend *backend);

static void tf_opengl_end_frame(TF_RendererBackend *backend);

static void tf_opengl_clear(TF_RendererBackend *backend, TF_ClearFlags flags);

static void tf_opengl_set_clear_color(TF_RendererBackend *backend, TF_Color color);

static void tf_opengl_set_viewport(TF_RendererBackend *backend, i32 x, i32 y, u32 width, u32 height);

static void tf_opengl_draw_triangle(TF_RendererBackend *backend, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color);

// VTable for OpenGL backend
static TF_RendererBackendVTable s_opengl_vtable = {
    .create = tf_opengl_create,
    .destroy = tf_opengl_destroy,
    .begin_frame = tf_opengl_begin_frame,
    .end_frame = tf_opengl_end_frame,
    .clear = tf_opengl_clear,
    .set_clear_color = tf_opengl_set_clear_color,
    .set_viewport = tf_opengl_set_viewport,
    .draw_triangle = tf_opengl_draw_triangle
};

// Backend creation function
TF_RendererBackend *tf_renderer_backend_create_opengl(void) {
    TF_DEBUG("Creating OpenGL renderer backend...");

    TF_RendererBackend *backend = malloc(sizeof(TF_RendererBackend));
    if (!backend) {
        TF_ERROR("Failed to allocate OpenGL backend");
        return TF_NULL;
    }

    backend->vtable = &s_opengl_vtable;
    backend->data = TF_NULL;

    return backend;
}

// OpenGL implementation functions
static b32 tf_opengl_create(TF_RendererBackend *backend, TF_Window *window) {
    TF_DEBUG("Initializing OpenGL backend...");
    // Initialize GLAD (new version)
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        TF_ERROR("Failed to initialize OpenGL context");
        return TF_FALSE;
    }
    // Log OpenGL info (new version format)
    TF_INFO("Loaded OpenGL %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    TF_INFO("OpenGL Vendor: %s", glGetString(GL_VENDOR));
    TF_INFO("OpenGL Renderer: %s", glGetString(GL_RENDERER));
    // Allocate OpenGL-specific data
    TF_OpenGLData *gl_data = malloc(sizeof(TF_OpenGLData));
    if (!gl_data) {
        TF_ERROR("Failed to allocate OpenGL data");
        return TF_FALSE;
    }
    backend->data = gl_data;
    // Initialize OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Set default clear color
    gl_data->clear_color = TF_COLOR_BLUE;
    glClearColor(gl_data->clear_color.r, gl_data->clear_color.g, gl_data->clear_color.b, gl_data->clear_color.a);
    // Create default VAO
    glGenVertexArrays(1, &gl_data->vao);
    glBindVertexArray(gl_data->vao);
    // Create default VBO
    glGenBuffers(1, &gl_data->vbo);

    gl_data->shader = 0;

    TF_INFO("OpenGL backend initialized successfully");
    return TF_TRUE;
}

static void tf_opengl_destroy(TF_RendererBackend *backend) {
    if (!backend || !backend->data) {
        return;
    }

    TF_DEBUG("Destroying OpenGL backend...");

    TF_OpenGLData *gl_data = (TF_OpenGLData *) backend->data;

    // Clean up OpenGL resources
    if (gl_data->vbo) {
        glDeleteBuffers(1, &gl_data->vbo);
    }

    if (gl_data->vao) {
        glDeleteVertexArrays(1, &gl_data->vao);
    }

    // Free OpenGL data
    free(gl_data);
    backend->data = TF_NULL;

    TF_INFO("OpenGL backend destroyed");
}

static void tf_opengl_begin_frame(TF_RendererBackend *backend) {
    if (!backend || !backend->data) {
        return;
    }

    // Nothing special needed for begin frame in basic OpenGL
}

static void tf_opengl_end_frame(TF_RendererBackend *backend) {
    if (!backend || !backend->data) {
        return;
    }

    // Nothing special needed for end frame in basic OpenGL
}

static void tf_opengl_clear(TF_RendererBackend *backend, TF_ClearFlags flags) {
    if (!backend || !backend->data) {
        return;
    }

    GLbitfield gl_flags = 0;

    if (flags & TF_CLEAR_COLOR) {
        gl_flags |= GL_COLOR_BUFFER_BIT;
    }

    if (flags & TF_CLEAR_DEPTH) {
        gl_flags |= GL_DEPTH_BUFFER_BIT;
    }

    glClear(gl_flags);
}

static void tf_opengl_set_clear_color(TF_RendererBackend *backend, TF_Color color) {
    if (!backend || !backend->data) {
        return;
    }

    TF_OpenGLData *gl_data = (TF_OpenGLData *) backend->data;
    gl_data->clear_color = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

static void tf_opengl_set_viewport(TF_RendererBackend *backend, i32 x, i32 y, u32 width, u32 height) {
    if (!backend || !backend->data) {
        return;
    }

    TF_OpenGLData *gl_data = (TF_OpenGLData *) backend->data;
    gl_data->viewport_x = x;
    gl_data->viewport_y = y;
    gl_data->viewport_width = width;
    gl_data->viewport_height = height;

    glViewport(x, y, width, height);
}

static void tf_opengl_draw_triangle(TF_RendererBackend *backend, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color) {
    if (!backend || !backend->data) {
        return;
    }

    // For now, just log that we're drawing a triangle
    // We'll implement the actual triangle rendering later with shaders
    TF_DEBUG_TRACE("Drawing triangle: (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)",
                   p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z);
}
