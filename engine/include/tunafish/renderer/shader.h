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

// Opaque shader handle
typedef struct TF_Shader TF_Shader;

// =============================================================================
// Shader lifecycle
// =============================================================================

// Create shader from GLSL source strings
TF_API TF_Shader *tf_shader_create(const char *vertex_source, const char *fragment_source);

// Destroy shader and free resources
TF_API void tf_shader_destroy(TF_Shader *shader);

// Check if shader is valid (compiled successfully)
TF_API b32 tf_shader_is_valid(const TF_Shader *shader);

// =============================================================================
// Shader binding
// =============================================================================

TF_API void tf_shader_bind(TF_Shader *shader);
TF_API void tf_shader_unbind(void);

// =============================================================================
// Uniform setters
// =============================================================================

TF_API void tf_shader_set_int(TF_Shader *shader, const char *name, i32 value);
TF_API void tf_shader_set_float(TF_Shader *shader, const char *name, f32 value);
TF_API void tf_shader_set_vec2(TF_Shader *shader, const char *name, TF_Vec2 value);
TF_API void tf_shader_set_vec3(TF_Shader *shader, const char *name, TF_Vec3 value);
TF_API void tf_shader_set_vec4(TF_Shader *shader, const char *name, TF_Vec4 value);
TF_API void tf_shader_set_mat4(TF_Shader *shader, const char *name, const TF_Mat4 *value);
TF_API void tf_shader_set_color(TF_Shader *shader, const char *name, TF_Color value);

// =============================================================================
// Shader program ID (for advanced usage)
// =============================================================================

TF_API u32 tf_shader_get_program_id(const TF_Shader *shader);

#ifdef __cplusplus
}
#endif
