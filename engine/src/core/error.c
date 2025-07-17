//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/error.h"
#include "tunafish/core/log.h"
#include <string.h>
#include <stdio.h>

static _Thread_local TF_ErrorCode s_error_code = TF_ERROR_NONE;
static _Thread_local char s_error_message[256] = {0};

TF_API void tf_error_set(TF_ErrorCode code, const char *message) {
    s_error_code = code;

    if (message) {
        strncpy(s_error_message, message, sizeof(s_error_message) - 1);
        s_error_message[sizeof(s_error_message) - 1] = '\0';
    } else {
        s_error_message[0] = '\0';
    }

    TF_ERROR("Error [%d]: %s", code, s_error_message);
}

TF_API TF_ErrorCode tf_error_get_code(void) {
    return s_error_code;
}

TF_API const char *tf_error_get_message(void) {
    return s_error_message;
}

TF_API void tf_error_clear(void) {
    s_error_code = TF_ERROR_NONE;
    s_error_message[0] = '\0';
}
