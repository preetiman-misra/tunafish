//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/renderer/backend/opengl/gl_renderer.h"
#include "tunafish/core/log.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

// =============================================================================
// Default shaders for triangle rendering
// =============================================================================

static const char *s_triangle_vertex_shader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 vertexColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    vertexColor = aColor;\n"
    "}\n";

static const char *s_triangle_fragment_shader =
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vertexColor;\n"
    "}\n";

// =============================================================================
// Forward declarations
// =============================================================================

static b32 tf_opengl_create(TF_RendererBackend *backend, TF_Window *window);
static void tf_opengl_destroy(TF_RendererBackend *backend);
static void tf_opengl_begin_frame(TF_RendererBackend *backend);
static void tf_opengl_end_frame(TF_RendererBackend *backend);
static void tf_opengl_clear(TF_RendererBackend *backend, TF_ClearFlags flags);
static void tf_opengl_set_clear_color(TF_RendererBackend *backend, TF_Color color);
static void tf_opengl_set_viewport(TF_RendererBackend *backend, i32 x, i32 y, u32 width, u32 height);
static void tf_opengl_draw_triangle(TF_RendererBackend *backend, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color);

// =============================================================================
// VTable
// =============================================================================

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

// =============================================================================
// Backend creation
// =============================================================================

TF_RendererBackend *tf_renderer_backend_create_opengl(void) {
    TF_DEBUG("Creating OpenGL renderer backend...");

    TF_RendererBackend *backend = malloc(sizeof(TF_RendererBackend));
    if (!backend) {
        TF_ERROR("Failed to allocate OpenGL backend");
        return NULL;
    }

    backend->vtable = &s_opengl_vtable;
    backend->data = NULL;

    return backend;
}

// =============================================================================
// Implementation
// =============================================================================

static b32 tf_opengl_create(TF_RendererBackend *backend, TF_Window *window) {
    TF_DEBUG("Initializing OpenGL backend...");

    // Initialize GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        TF_ERROR("Failed to initialize OpenGL context");
        return TF_FALSE;
    }

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
    glClearColor(gl_data->clear_color.r, gl_data->clear_color.g,
                 gl_data->clear_color.b, gl_data->clear_color.a);

    // Create triangle shader
    gl_data->triangle_shader = tf_shader_create(s_triangle_vertex_shader, s_triangle_fragment_shader);
    if (!gl_data->triangle_shader) {
        TF_ERROR("Failed to create triangle shader");
        free(gl_data);
        return TF_FALSE;
    }

    // Create VAO for triangle rendering
    glGenVertexArrays(1, &gl_data->triangle_vao);
    glBindVertexArray(gl_data->triangle_vao);

    // Create VBO for triangle vertices (position + color per vertex)
    // 3 vertices * (3 floats position + 4 floats color) = 21 floats
    glGenBuffers(1, &gl_data->triangle_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl_data->triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 21, NULL, GL_DYNAMIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(f32), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    TF_INFO("OpenGL backend initialized successfully");
    return TF_TRUE;
}

static void tf_opengl_destroy(TF_RendererBackend *backend) {
    if (!backend || !backend->data) return;

    TF_DEBUG("Destroying OpenGL backend...");

    TF_OpenGLData *gl_data = (TF_OpenGLData *)backend->data;

    // Clean up OpenGL resources
    if (gl_data->triangle_vbo) {
        glDeleteBuffers(1, &gl_data->triangle_vbo);
    }
    if (gl_data->triangle_vao) {
        glDeleteVertexArrays(1, &gl_data->triangle_vao);
    }
    if (gl_data->triangle_shader) {
        tf_shader_destroy(gl_data->triangle_shader);
    }

    free(gl_data);
    backend->data = NULL;

    TF_INFO("OpenGL backend destroyed");
}

static void tf_opengl_begin_frame(TF_RendererBackend *backend) {
    (void)backend;
    // Nothing special needed for begin frame
}

static void tf_opengl_end_frame(TF_RendererBackend *backend) {
    (void)backend;
    // Nothing special needed for end frame
}

static void tf_opengl_clear(TF_RendererBackend *backend, TF_ClearFlags flags) {
    (void)backend;

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
    if (!backend || !backend->data) return;

    TF_OpenGLData *gl_data = (TF_OpenGLData *)backend->data;
    gl_data->clear_color = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

static void tf_opengl_set_viewport(TF_RendererBackend *backend, i32 x, i32 y, u32 width, u32 height) {
    if (!backend || !backend->data) return;

    TF_OpenGLData *gl_data = (TF_OpenGLData *)backend->data;
    gl_data->viewport_x = x;
    gl_data->viewport_y = y;
    gl_data->viewport_width = width;
    gl_data->viewport_height = height;

    glViewport(x, y, width, height);
}

static void tf_opengl_draw_triangle(TF_RendererBackend *backend, TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color) {
    if (!backend || !backend->data) return;

    TF_OpenGLData *gl_data = (TF_OpenGLData *)backend->data;

    // Vertex data: position (3) + color (4) per vertex
    f32 vertices[] = {
        // Vertex 1
        p1.x, p1.y, p1.z,  color.r, color.g, color.b, color.a,
        // Vertex 2
        p2.x, p2.y, p2.z,  color.r, color.g, color.b, color.a,
        // Vertex 3
        p3.x, p3.y, p3.z,  color.r, color.g, color.b, color.a
    };

    // Update VBO with new vertex data
    glBindBuffer(GL_ARRAY_BUFFER, gl_data->triangle_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Bind shader and VAO, then draw
    tf_shader_bind(gl_data->triangle_shader);
    glBindVertexArray(gl_data->triangle_vao);

    // Ensure proper GL state for 2D triangle rendering
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    tf_shader_unbind();
}
