//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/renderer/shader.h"
#include "tunafish/core/log.h"
#include <glad/gl.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// Shader structure
// =============================================================================

struct TF_Shader {
    u32 program_id;
    b32 valid;
};

// =============================================================================
// Internal helpers
// =============================================================================

static u32 compile_shader(GLenum type, const char *source) {
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check compilation status
    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        const char *type_str = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        TF_ERROR("Shader compilation failed (%s): %s", type_str, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static u32 link_program(u32 vertex_shader, u32 fragment_shader) {
    u32 program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Check link status
    i32 success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        TF_ERROR("Shader program linking failed: %s", info_log);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

// =============================================================================
// Shader lifecycle
// =============================================================================

TF_API TF_Shader *tf_shader_create(const char *vertex_source, const char *fragment_source) {
    if (!vertex_source || !fragment_source) {
        TF_ERROR("Shader source cannot be null");
        return NULL;
    }

    TF_Shader *shader = (TF_Shader *)malloc(sizeof(TF_Shader));
    if (!shader) {
        TF_ERROR("Failed to allocate shader");
        return NULL;
    }

    shader->program_id = 0;
    shader->valid = TF_FALSE;

    // Compile vertex shader
    u32 vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    if (vertex_shader == 0) {
        free(shader);
        return NULL;
    }

    // Compile fragment shader
    u32 fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        free(shader);
        return NULL;
    }

    // Link program
    shader->program_id = link_program(vertex_shader, fragment_shader);

    // Shaders are no longer needed after linking
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (shader->program_id == 0) {
        free(shader);
        return NULL;
    }

    shader->valid = TF_TRUE;
    TF_DEBUG("Shader created successfully (program ID: %u)", shader->program_id);
    return shader;
}

TF_API void tf_shader_destroy(TF_Shader *shader) {
    if (!shader) return;

    if (shader->program_id != 0) {
        glDeleteProgram(shader->program_id);
    }

    free(shader);
    TF_DEBUG("Shader destroyed");
}

TF_API b32 tf_shader_is_valid(const TF_Shader *shader) {
    return shader && shader->valid;
}

// =============================================================================
// Shader binding
// =============================================================================

TF_API void tf_shader_bind(TF_Shader *shader) {
    if (shader && shader->valid) {
        glUseProgram(shader->program_id);
    }
}

TF_API void tf_shader_unbind(void) {
    glUseProgram(0);
}

// =============================================================================
// Uniform setters
// =============================================================================

TF_API void tf_shader_set_int(TF_Shader *shader, const char *name, i32 value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

TF_API void tf_shader_set_float(TF_Shader *shader, const char *name, f32 value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

TF_API void tf_shader_set_vec2(TF_Shader *shader, const char *name, TF_Vec2 value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform2f(location, value.x, value.y);
    }
}

TF_API void tf_shader_set_vec3(TF_Shader *shader, const char *name, TF_Vec3 value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    }
}

TF_API void tf_shader_set_vec4(TF_Shader *shader, const char *name, TF_Vec4 value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
}

TF_API void tf_shader_set_mat4(TF_Shader *shader, const char *name, const TF_Mat4 *value) {
    if (!shader || !shader->valid || !name || !value) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, value->m);
    }
}

TF_API void tf_shader_set_color(TF_Shader *shader, const char *name, TF_Color value) {
    if (!shader || !shader->valid || !name) return;
    i32 location = glGetUniformLocation(shader->program_id, name);
    if (location != -1) {
        glUniform4f(location, value.r, value.g, value.b, value.a);
    }
}

// =============================================================================
// Shader program ID
// =============================================================================

TF_API u32 tf_shader_get_program_id(const TF_Shader *shader) {
    return shader ? shader->program_id : 0;
}
