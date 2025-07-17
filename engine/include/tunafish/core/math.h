//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// Math constants
#define TF_PI         3.14159265358979323846f
#define TF_PI_2       1.57079632679489661923f
#define TF_PI_4       0.78539816339744830962f
#define TF_DEG_TO_RAD 0.01745329251994329577f
#define TF_RAD_TO_DEG 57.29577951308232087680f

// Vector types
typedef struct {
    f32 x, y;
} TF_Vec2;

typedef struct {
    f32 x, y, z;
} TF_Vec3;

typedef struct {
    f32 x, y, z, w;
} TF_Vec4;

// Matrix type (column-major order: m[col*4+row])
typedef struct {
    f32 m[16]; // 4x4 matrix stored in column-major order [col*4+row]
} TF_Mat4;

// Vector2 operations
TF_API TF_Vec2 tf_vec2_create(f32 x, f32 y);

TF_API TF_Vec2 tf_vec2_add(TF_Vec2 a, TF_Vec2 b);

TF_API TF_Vec2 tf_vec2_sub(TF_Vec2 a, TF_Vec2 b);

TF_API TF_Vec2 tf_vec2_scale(TF_Vec2 v, f32 s);

TF_API f32 tf_vec2_dot(TF_Vec2 a, TF_Vec2 b);

TF_API f32 tf_vec2_length(TF_Vec2 v);

TF_API TF_Vec2 tf_vec2_normalize(TF_Vec2 v);

// Vector3 operations
TF_API TF_Vec3 tf_vec3_create(f32 x, f32 y, f32 z);

TF_API TF_Vec3 tf_vec3_add(TF_Vec3 a, TF_Vec3 b);

TF_API TF_Vec3 tf_vec3_sub(TF_Vec3 a, TF_Vec3 b);

TF_API TF_Vec3 tf_vec3_scale(TF_Vec3 v, f32 s);

TF_API f32 tf_vec3_dot(TF_Vec3 a, TF_Vec3 b);

TF_API TF_Vec3 tf_vec3_cross(TF_Vec3 a, TF_Vec3 b);

TF_API f32 tf_vec3_length(TF_Vec3 v);

TF_API TF_Vec3 tf_vec3_normalize(TF_Vec3 v);

// Vector4 operations
TF_API TF_Vec4 tf_vec4_create(f32 x, f32 y, f32 z, f32 w);

TF_API TF_Vec4 tf_vec4_add(TF_Vec4 a, TF_Vec4 b);

TF_API TF_Vec4 tf_vec4_sub(TF_Vec4 a, TF_Vec4 b);

TF_API TF_Vec4 tf_vec4_scale(TF_Vec4 v, f32 s);

TF_API f32 tf_vec4_dot(TF_Vec4 a, TF_Vec4 b);

// Matrix operations
TF_API TF_Mat4 tf_mat4_identity(void);

TF_API TF_Mat4 tf_mat4_multiply(TF_Mat4 a, TF_Mat4 b);

TF_API TF_Vec4 tf_mat4_multiply_vec4(TF_Mat4 m, TF_Vec4 v);

TF_API TF_Mat4 tf_mat4_transpose(TF_Mat4 m);

// Transform matrices
TF_API TF_Mat4 tf_mat4_translate(TF_Vec3 translation);

TF_API TF_Mat4 tf_mat4_rotate_x(f32 angle_radians);

TF_API TF_Mat4 tf_mat4_rotate_y(f32 angle_radians);

TF_API TF_Mat4 tf_mat4_rotate_z(f32 angle_radians);

TF_API TF_Mat4 tf_mat4_scale(TF_Vec3 scale);

// Projection matrices
TF_API TF_Mat4 tf_mat4_perspective(f32 fov_radians, f32 aspect, f32 near_plane, f32 far_plane);

TF_API TF_Mat4 tf_mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);

TF_API TF_Mat4 tf_mat4_look_at(TF_Vec3 eye, TF_Vec3 target, TF_Vec3 up);

// Utility functions
TF_API f32 tf_radians(f32 degrees);

TF_API f32 tf_degrees(f32 radians);

TF_API f32 tf_clamp(f32 value, f32 min_val, f32 max_val);

TF_API f32 tf_lerp(f32 a, f32 b, f32 t);

#ifdef __cplusplus
}
#endif
