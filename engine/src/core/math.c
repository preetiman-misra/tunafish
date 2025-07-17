//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/math.h"
#include <math.h>
#include <string.h>

// Vector2 operations
TF_API TF_Vec2 tf_vec2_create(f32 x, f32 y) {
    TF_Vec2 result = {x, y};
    return result;
}

TF_API TF_Vec2 tf_vec2_add(TF_Vec2 a, TF_Vec2 b) {
    TF_Vec2 result = {a.x + b.x, a.y + b.y};
    return result;
}

TF_API TF_Vec2 tf_vec2_sub(TF_Vec2 a, TF_Vec2 b) {
    TF_Vec2 result = {a.x - b.x, a.y - b.y};
    return result;
}

TF_API TF_Vec2 tf_vec2_scale(TF_Vec2 v, f32 s) {
    TF_Vec2 result = {v.x * s, v.y * s};
    return result;
}

TF_API f32 tf_vec2_dot(TF_Vec2 a, TF_Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

TF_API f32 tf_vec2_length(TF_Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

TF_API TF_Vec2 tf_vec2_normalize(TF_Vec2 v) {
    f32 length = tf_vec2_length(v);
    if (length > 0.0001f) {
        // Avoid division by zero
        return tf_vec2_scale(v, 1.0f / length);
    }
    return tf_vec2_create(0.0f, 0.0f);
}

// Vector3 operations
TF_API TF_Vec3 tf_vec3_create(f32 x, f32 y, f32 z) {
    TF_Vec3 result = {x, y, z};
    return result;
}

TF_API TF_Vec3 tf_vec3_add(TF_Vec3 a, TF_Vec3 b) {
    TF_Vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

TF_API TF_Vec3 tf_vec3_sub(TF_Vec3 a, TF_Vec3 b) {
    TF_Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

TF_API TF_Vec3 tf_vec3_scale(TF_Vec3 v, f32 s) {
    TF_Vec3 result = {v.x * s, v.y * s, v.z * s};
    return result;
}

TF_API f32 tf_vec3_dot(TF_Vec3 a, TF_Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

TF_API TF_Vec3 tf_vec3_cross(TF_Vec3 a, TF_Vec3 b) {
    TF_Vec3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

TF_API f32 tf_vec3_length(TF_Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

TF_API TF_Vec3 tf_vec3_normalize(TF_Vec3 v) {
    f32 length = tf_vec3_length(v);
    if (length > 0.0001f) {
        // Avoid division by zero
        return tf_vec3_scale(v, 1.0f / length);
    }
    return tf_vec3_create(0.0f, 0.0f, 0.0f);
}

// Vector4 operations
TF_API TF_Vec4 tf_vec4_create(f32 x, f32 y, f32 z, f32 w) {
    TF_Vec4 result = {x, y, z, w};
    return result;
}

TF_API TF_Vec4 tf_vec4_add(TF_Vec4 a, TF_Vec4 b) {
    TF_Vec4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return result;
}

TF_API TF_Vec4 tf_vec4_sub(TF_Vec4 a, TF_Vec4 b) {
    TF_Vec4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return result;
}

TF_API TF_Vec4 tf_vec4_scale(TF_Vec4 v, f32 s) {
    TF_Vec4 result = {v.x * s, v.y * s, v.z * s, v.w * s};
    return result;
}

TF_API f32 tf_vec4_dot(TF_Vec4 a, TF_Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Matrix operations
TF_API TF_Mat4 tf_mat4_identity(void) {
    TF_Mat4 result = {0};
    result.m[0] = 1.0f; // [0][0]
    result.m[5] = 1.0f; // [1][1]
    result.m[10] = 1.0f; // [2][2]
    result.m[15] = 1.0f; // [3][3]
    return result;
}

// FIXED: Column-major matrix multiplication
TF_API TF_Mat4 tf_mat4_multiply(TF_Mat4 a, TF_Mat4 b) {
    TF_Mat4 result = {0};

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int k = 0; k < 4; k++) {
                result.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
        }
    }

    return result;
}

TF_API TF_Vec4 tf_mat4_multiply_vec4(TF_Mat4 m, TF_Vec4 v) {
    TF_Vec4 result = {
        m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
        m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    };
    return result;
}

TF_API TF_Mat4 tf_mat4_transpose(TF_Mat4 m) {
    TF_Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = m.m[j * 4 + i];
        }
    }
    return result;
}

// Transform matrices
TF_API TF_Mat4 tf_mat4_translate(TF_Vec3 translation) {
    TF_Mat4 result = tf_mat4_identity();
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

// FIXED: Rotation matrices with correct signs
TF_API TF_Mat4 tf_mat4_rotate_x(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);

    result.m[5] = c; // [1][1]
    result.m[6] = -s; // [1][2] - Fixed sign
    result.m[9] = s; // [2][1] - Fixed sign
    result.m[10] = c; // [2][2]

    return result;
}

TF_API TF_Mat4 tf_mat4_rotate_y(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);

    result.m[0] = c; // [0][0]
    result.m[2] = s; // [0][2] - Fixed sign
    result.m[8] = -s; // [2][0] - Fixed sign
    result.m[10] = c; // [2][2]

    return result;
}

TF_API TF_Mat4 tf_mat4_rotate_z(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);

    result.m[0] = c; // [0][0]
    result.m[1] = -s; // [0][1] - Fixed sign
    result.m[4] = s; // [1][0] - Fixed sign
    result.m[5] = c; // [1][1]

    return result;
}

TF_API TF_Mat4 tf_mat4_scale(TF_Vec3 scale) {
    TF_Mat4 result = tf_mat4_identity();
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

// FIXED: Projection matrices with correct OpenGL format
TF_API TF_Mat4 tf_mat4_perspective(f32 fov_radians, f32 aspect, f32 near_plane, f32 far_plane) {
    TF_Mat4 result = {0};

    f32 tan_half_fov = tanf(fov_radians * 0.5f);
    f32 range = far_plane - near_plane;

    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;
    result.m[10] = -(far_plane + near_plane) / range;
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far_plane * near_plane) / range;

    return result;
}

TF_API TF_Mat4 tf_mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane) {
    TF_Mat4 result = {0};

    f32 width = right - left;
    f32 height = top - bottom;
    f32 depth = far_plane - near_plane;

    result.m[0] = 2.0f / width;
    result.m[5] = 2.0f / height;
    result.m[10] = -2.0f / depth;
    result.m[12] = -(right + left) / width;
    result.m[13] = -(top + bottom) / height;
    result.m[14] = -(far_plane + near_plane) / depth;
    result.m[15] = 1.0f;

    return result;
}

// FIXED: Look-at matrix with proper forward direction
TF_API TF_Mat4 tf_mat4_look_at(TF_Vec3 eye, TF_Vec3 target, TF_Vec3 up) {
    TF_Vec3 forward = tf_vec3_normalize(tf_vec3_sub(target, eye));
    TF_Vec3 right = tf_vec3_normalize(tf_vec3_cross(forward, up));
    TF_Vec3 up_corrected = tf_vec3_cross(right, forward);

    TF_Mat4 result = {0};

    // Rotation part
    result.m[0] = right.x;
    result.m[4] = right.y;
    result.m[8] = right.z;
    result.m[1] = up_corrected.x;
    result.m[5] = up_corrected.y;
    result.m[9] = up_corrected.z;
    result.m[2] = -forward.x;
    result.m[6] = -forward.y;
    result.m[10] = -forward.z;

    // Translation part
    result.m[12] = -tf_vec3_dot(right, eye);
    result.m[13] = -tf_vec3_dot(up_corrected, eye);
    result.m[14] = tf_vec3_dot(forward, eye);
    result.m[15] = 1.0f;

    return result;
}

// Utility functions
TF_API f32 tf_radians(f32 degrees) {
    return degrees * TF_DEG_TO_RAD;
}

TF_API f32 tf_degrees(f32 radians) {
    return radians * TF_RAD_TO_DEG;
}

TF_API f32 tf_clamp(f32 value, f32 min_val, f32 max_val) {
    return value < min_val ? min_val : (value > max_val ? max_val : value);
}

TF_API f32 tf_lerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}
