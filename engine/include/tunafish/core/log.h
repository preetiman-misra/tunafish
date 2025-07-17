//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

// Log levels
typedef enum {
    TF_LOG_LEVEL_TRACE = 0,
    TF_LOG_LEVEL_DEBUG = 1,
    TF_LOG_LEVEL_INFO = 2,
    TF_LOG_LEVEL_WARN = 3,
    TF_LOG_LEVEL_ERROR = 4,
    TF_LOG_LEVEL_FATAL = 5
} TF_LogLevel;

// Log output destinations
typedef enum {
    TF_LOG_OUTPUT_CONSOLE = 1 << 0,
    TF_LOG_OUTPUT_FILE = 1 << 1
} TF_LogOutput;

// Logging API
TF_API void tf_log_init(TF_LogLevel min_level, u32 output_flags, const char *log_file);

TF_API void tf_log_shutdown(void);

TF_API void tf_log_message(TF_LogLevel level, const char *file, u32 line, const char *format, ...);

// Convenience macros
#define TF_LOG_TRACE(format, ...) tf_log_message(TF_LOG_LEVEL_TRACE, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TF_LOG_DEBUG(format, ...) tf_log_message(TF_LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TF_LOG_INFO(format, ...)  tf_log_message(TF_LOG_LEVEL_INFO,  __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TF_LOG_WARN(format, ...)  tf_log_message(TF_LOG_LEVEL_WARN,  __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TF_LOG_ERROR(format, ...) tf_log_message(TF_LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TF_LOG_FATAL(format, ...) tf_log_message(TF_LOG_LEVEL_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

// Short aliases (avoiding TF_DEBUG conflict)
#define TF_TRACE TF_LOG_TRACE
#define TF_DEBUG TF_LOG_DEBUG
#define TF_INFO  TF_LOG_INFO
#define TF_WARN  TF_LOG_WARN
#define TF_ERROR TF_LOG_ERROR
#define TF_FATAL TF_LOG_FATAL

// Conditional logging (only in debug builds)
#if TF_IS_DEBUG
#define TF_DEBUG_TRACE(format, ...) TF_TRACE(format, ##__VA_ARGS__)
#define TF_DEBUG_DEBUG(format, ...) TF_DEBUG(format, ##__VA_ARGS__)
#else
#define TF_DEBUG_TRACE(format, ...)
#define TF_DEBUG_DEBUG(format, ...)
#endif

#ifdef __cplusplus
}
#endif
