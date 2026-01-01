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

// =============================================================================
// Inline Vector2 operations
// =============================================================================

static inline TF_Vec2 tf_vec2_create(f32 x, f32 y) {
    return (TF_Vec2){x, y};
}

static inline TF_Vec2 tf_vec2_add(TF_Vec2 a, TF_Vec2 b) {
    return (TF_Vec2){a.x + b.x, a.y + b.y};
}

static inline TF_Vec2 tf_vec2_sub(TF_Vec2 a, TF_Vec2 b) {
    return (TF_Vec2){a.x - b.x, a.y - b.y};
}

static inline TF_Vec2 tf_vec2_scale(TF_Vec2 v, f32 s) {
    return (TF_Vec2){v.x * s, v.y * s};
}

static inline f32 tf_vec2_dot(TF_Vec2 a, TF_Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline f32 tf_vec2_length(TF_Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline TF_Vec2 tf_vec2_normalize(TF_Vec2 v) {
    f32 len = sqrtf(v.x * v.x + v.y * v.y);
    if (len > 0.0001f) {
        f32 inv_len = 1.0f / len;
        return (TF_Vec2){v.x * inv_len, v.y * inv_len};
    }
    return (TF_Vec2){0.0f, 0.0f};
}

// =============================================================================
// Inline Vector3 operations
// =============================================================================

static inline TF_Vec3 tf_vec3_create(f32 x, f32 y, f32 z) {
    return (TF_Vec3){x, y, z};
}

static inline TF_Vec3 tf_vec3_add(TF_Vec3 a, TF_Vec3 b) {
    return (TF_Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline TF_Vec3 tf_vec3_sub(TF_Vec3 a, TF_Vec3 b) {
    return (TF_Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline TF_Vec3 tf_vec3_scale(TF_Vec3 v, f32 s) {
    return (TF_Vec3){v.x * s, v.y * s, v.z * s};
}

static inline f32 tf_vec3_dot(TF_Vec3 a, TF_Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline TF_Vec3 tf_vec3_cross(TF_Vec3 a, TF_Vec3 b) {
    return (TF_Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline f32 tf_vec3_length(TF_Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline TF_Vec3 tf_vec3_normalize(TF_Vec3 v) {
    f32 len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0.0001f) {
        f32 inv_len = 1.0f / len;
        return (TF_Vec3){v.x * inv_len, v.y * inv_len, v.z * inv_len};
    }
    return (TF_Vec3){0.0f, 0.0f, 0.0f};
}

// =============================================================================
// Inline Vector4 operations
// =============================================================================

static inline TF_Vec4 tf_vec4_create(f32 x, f32 y, f32 z, f32 w) {
    return (TF_Vec4){x, y, z, w};
}

static inline TF_Vec4 tf_vec4_add(TF_Vec4 a, TF_Vec4 b) {
    return (TF_Vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

static inline TF_Vec4 tf_vec4_sub(TF_Vec4 a, TF_Vec4 b) {
    return (TF_Vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

static inline TF_Vec4 tf_vec4_scale(TF_Vec4 v, f32 s) {
    return (TF_Vec4){v.x * s, v.y * s, v.z * s, v.w * s};
}

static inline f32 tf_vec4_dot(TF_Vec4 a, TF_Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// =============================================================================
// Inline utility functions
// =============================================================================

static inline f32 tf_radians(f32 degrees) {
    return degrees * TF_DEG_TO_RAD;
}

static inline f32 tf_degrees(f32 radians) {
    return radians * TF_RAD_TO_DEG;
}

static inline f32 tf_clamp(f32 value, f32 min_val, f32 max_val) {
    return value < min_val ? min_val : (value > max_val ? max_val : value);
}

static inline f32 tf_lerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}

// =============================================================================
// Inline matrix operations (commonly used in hot paths)
// =============================================================================

static inline TF_Mat4 tf_mat4_identity(void) {
    return (TF_Mat4){{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};
}

static inline TF_Vec4 tf_mat4_multiply_vec4(TF_Mat4 m, TF_Vec4 v) {
    return (TF_Vec4){
        m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
        m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    };
}

// =============================================================================
// Non-inline matrix operations (less frequent, more complex)
// =============================================================================

TF_API TF_Mat4 tf_mat4_multiply(TF_Mat4 a, TF_Mat4 b);
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

#ifdef __cplusplus
}
#endif
