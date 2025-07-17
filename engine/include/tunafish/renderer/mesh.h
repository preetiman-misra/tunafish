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

// Forward declarations
typedef struct TF_Mesh TF_Mesh;

// Simple mesh creation
TF_API TF_Mesh *tf_mesh_create_triangle(TF_Vec3 p1, TF_Vec3 p2, TF_Vec3 p3, TF_Color color);

TF_API TF_Mesh *tf_mesh_create_cube(f32 size);

TF_API void tf_mesh_destroy(TF_Mesh *mesh);

#ifdef __cplusplus
}
#endif
