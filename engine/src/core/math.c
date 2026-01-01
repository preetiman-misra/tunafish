//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/math.h"

// =============================================================================
// Matrix operations (kept non-inline due to complexity)
// =============================================================================

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

TF_API TF_Mat4 tf_mat4_transpose(TF_Mat4 m) {
    TF_Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = m.m[j * 4 + i];
        }
    }
    return result;
}

// =============================================================================
// Transform matrices
// =============================================================================

TF_API TF_Mat4 tf_mat4_translate(TF_Vec3 translation) {
    TF_Mat4 result = tf_mat4_identity();
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

TF_API TF_Mat4 tf_mat4_rotate_x(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);
    result.m[5] = c;
    result.m[6] = -s;
    result.m[9] = s;
    result.m[10] = c;
    return result;
}

TF_API TF_Mat4 tf_mat4_rotate_y(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);
    result.m[0] = c;
    result.m[2] = s;
    result.m[8] = -s;
    result.m[10] = c;
    return result;
}

TF_API TF_Mat4 tf_mat4_rotate_z(f32 angle_radians) {
    TF_Mat4 result = tf_mat4_identity();
    f32 c = cosf(angle_radians);
    f32 s = sinf(angle_radians);
    result.m[0] = c;
    result.m[1] = -s;
    result.m[4] = s;
    result.m[5] = c;
    return result;
}

TF_API TF_Mat4 tf_mat4_scale(TF_Vec3 scale) {
    TF_Mat4 result = tf_mat4_identity();
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

// =============================================================================
// Projection matrices
// =============================================================================

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
