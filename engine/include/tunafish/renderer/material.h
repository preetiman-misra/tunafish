//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"
#include "tunafish/renderer/renderer_types.h"
#include "tunafish/renderer/shader.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct TF_Material TF_Material;

// Material lifecycle
TF_API TF_Material *tf_material_create_default(void);

TF_API void tf_material_destroy(TF_Material *material);

// Property management
TF_API void tf_material_set_color(TF_Material *material, TF_Color color);

#ifdef __cplusplus
}
#endif
