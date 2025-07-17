//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct TF_Texture TF_Texture;

// Texture lifecycle
TF_API TF_Texture *tf_texture_create_white(void);

TF_API void tf_texture_destroy(TF_Texture *texture);

// Texture binding
TF_API void tf_texture_bind(TF_Texture *texture, u32 slot);

#ifdef __cplusplus
}
#endif
