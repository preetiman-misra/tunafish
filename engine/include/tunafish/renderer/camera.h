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
typedef struct TF_Camera TF_Camera;

// Camera lifecycle
TF_API TF_Camera *tf_camera_create_perspective(f32 fov_degrees, f32 aspect_ratio, f32 near_plane, f32 far_plane);

TF_API void tf_camera_destroy(TF_Camera *camera);

// Transform operations
TF_API void tf_camera_set_position(TF_Camera *camera, TF_Vec3 position);

TF_API void tf_camera_set_look_at(TF_Camera *camera, TF_Vec3 eye, TF_Vec3 target, TF_Vec3 up);

// Matrix access
TF_API TF_Mat4 tf_camera_get_view_matrix(const TF_Camera *camera);

TF_API TF_Mat4 tf_camera_get_projection_matrix(const TF_Camera *camera);

#ifdef __cplusplus
}
#endif
