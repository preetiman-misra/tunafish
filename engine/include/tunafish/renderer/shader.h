//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"
#include "tunafish/core/math.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct TF_Shader TF_Shader;

// Shader lifecycle
TF_API TF_Shader *tf_shader_create_from_strings(const char *vertex_source, const char *fragment_source);

TF_API void tf_shader_destroy(TF_Shader *shader);

// Shader binding
TF_API void tf_shader_bind(TF_Shader *shader);

// Uniform setting
TF_API void tf_shader_set_uniform_mat4(TF_Shader *shader, const char *name, TF_Mat4 value);

TF_API void tf_shader_set_uniform_vec3(TF_Shader *shader, const char *name, TF_Vec3 value);

// Shader information
TF_API b32 tf_shader_is_valid(const TF_Shader *shader);

#ifdef __cplusplus
}
#endif
