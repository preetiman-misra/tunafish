//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/platform/window.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>

// Window structure (implementation details)
struct TF_Window {
    GLFWwindow *glfw_window;
    u32 width;
    u32 height;
    char *title;
};

// Global GLFW initialization state
static b32 s_glfw_initialized = TF_FALSE;
static u32 s_window_count = 0;

// Initialize GLFW if not already done
static b32 tf_window_init_glfw(void) {
    if (s_glfw_initialized) {
        return TF_TRUE;
    }

    if (!glfwInit()) {
        return TF_FALSE;
    }

    s_glfw_initialized = TF_TRUE;
    return TF_TRUE;
}

// Shutdown GLFW when no windows remain
static void tf_window_shutdown_glfw(void) {
    if (s_glfw_initialized && s_window_count == 0) {
        glfwTerminate();
        s_glfw_initialized = TF_FALSE;
    }
}

TF_API TF_Window *tf_window_create(const TF_WindowConfig *config) {
    if (!config) {
        return TF_NULL;
    }

    // Initialize GLFW
    if (!tf_window_init_glfw()) {
        return TF_NULL;
    }

    // Set window hints
    glfwWindowHint(GLFW_RESIZABLE, config->resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    GLFWmonitor *monitor = config->fullscreen ? glfwGetPrimaryMonitor() : TF_NULL;
    GLFWwindow *glfw_window = glfwCreateWindow(
        (int) config->width,
        (int) config->height,
        config->title,
        monitor,
        TF_NULL
    );

    if (!glfw_window) {
        tf_window_shutdown_glfw();
        return TF_NULL;
    }

    // Create TF_Window
    TF_Window *window = (TF_Window *) malloc(sizeof(TF_Window));
    if (!window) {
        glfwDestroyWindow(glfw_window);
        tf_window_shutdown_glfw();
        return TF_NULL;
    }

    window->glfw_window = glfw_window;
    window->width = config->width;
    window->height = config->height;
    window->title = TF_NULL; // We'll set this if needed

    // Make context current
    glfwMakeContextCurrent(glfw_window);

    s_window_count++;
    return window;
}

TF_API void tf_window_destroy(TF_Window *window) {
    if (!window) return;

    if (window->glfw_window) {
        glfwDestroyWindow(window->glfw_window);
    }

    if (window->title) {
        free(window->title);
    }

    free(window);

    s_window_count--;
    tf_window_shutdown_glfw();
}

TF_API b32 tf_window_should_close(TF_Window *window) {
    if (!window || !window->glfw_window) {
        return TF_TRUE;
    }

    return glfwWindowShouldClose(window->glfw_window) ? TF_TRUE : TF_FALSE;
}

TF_API void tf_window_poll_events(TF_Window *window) {
    (void) window; // Unused parameter
    glfwPollEvents();
}

TF_API void tf_window_swap_buffers(TF_Window *window) {
    if (!window || !window->glfw_window) {
        return;
    }

    glfwSwapBuffers(window->glfw_window);
}

TF_API void tf_window_get_size(TF_Window *window, u32 *width, u32 *height) {
    if (!window || !window->glfw_window) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (width) *width = window->width;
    if (height) *height = window->height;
}

TF_API void tf_window_set_title(TF_Window *window, const char *title) {
    if (!window || !window->glfw_window || !title) {
        return;
    }

    glfwSetWindowTitle(window->glfw_window, title);
}
