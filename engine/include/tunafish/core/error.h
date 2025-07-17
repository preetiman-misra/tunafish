//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TF_ERROR_NONE = 0,
    TF_ERROR_UNKNOWN,
    TF_ERROR_INVALID_ARGUMENT,
    TF_ERROR_OUT_OF_MEMORY,
    TF_ERROR_FILE_NOT_FOUND,
    TF_ERROR_FILE_IO,
    TF_ERROR_NOT_IMPLEMENTED,
    TF_ERROR_COUNT
} TF_ErrorCode;

TF_API void tf_error_set(TF_ErrorCode code, const char *message);

TF_API TF_ErrorCode tf_error_get_code(void);

TF_API const char *tf_error_get_message(void);

TF_API void tf_error_clear(void);
