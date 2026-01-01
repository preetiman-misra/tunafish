//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/core/log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

// Log state
static struct {
    TF_LogLevel min_level;
    u32 output_flags;
    FILE *log_file;
    b32 initialized;
} s_log_state = {0};

// Log level strings
static const char *s_log_level_strings[] = {
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};

// Log level colors (ANSI escape codes)
static const char *s_log_level_colors[] = {
    "\033[37m", // TRACE - White
    "\033[36m", // DEBUG - Cyan
    "\033[32m", // INFO  - Green
    "\033[33m", // WARN  - Yellow
    "\033[31m", // ERROR - Red
    "\033[35m" // FATAL - Magenta
};

static const char *s_color_reset = "\033[0m";

// Get current timestamp string
static void tf_log_get_timestamp(char *buffer, usize buffer_size) {
    time_t raw_time;

    time(&raw_time);
    const struct tm *time_info = localtime(&raw_time);

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_info);
}

// Extract filename from full path
static const char *tf_log_get_filename(const char *path) {
    const char *filename = strrchr(path, '/');
    if (!filename) {
        filename = strrchr(path, '\\'); // Windows path separator
    }
    return filename ? filename + 1 : path;
}


TF_API void tf_log_init(TF_LogLevel min_level, u32 output_flags, const char *log_file) {
    s_log_state.min_level = min_level;
    s_log_state.output_flags = output_flags;
    s_log_state.log_file = TF_NULL;
    s_log_state.initialized = TF_TRUE;

    // Open log file if requested
    if ((output_flags & TF_LOG_OUTPUT_FILE) && log_file) {
        s_log_state.log_file = fopen(log_file, "w");
        if (!s_log_state.log_file) {
            fprintf(stderr, "WARNING: Failed to open log file '%s'\n", log_file);
        }
    }

    TF_INFO("Logging system initialized");
}

TF_API void tf_log_shutdown(void) {
    if (!s_log_state.initialized) {
        return;
    }
    TF_INFO("Logging system shutdown");

    if (s_log_state.log_file) {
        fclose(s_log_state.log_file);
        s_log_state.log_file = TF_NULL;
    }

    s_log_state.initialized = TF_FALSE;
}

TF_API void tf_log_message(TF_LogLevel level, const char *file, u32 line, const char *format, ...) {
    if (!s_log_state.initialized || level < s_log_state.min_level) {
        return;
    }
    // Get timestamp
    char timestamp[64];
    tf_log_get_timestamp(timestamp, sizeof(timestamp));
    // Get filename without path
    const char *filename = tf_log_get_filename(file);

    // Console output
    if (s_log_state.output_flags & TF_LOG_OUTPUT_CONSOLE) {
        va_list args;
        va_start(args, format);
        printf("%s[%s]%s [%s] %s:%u: ", s_log_level_colors[level],
               s_log_level_strings[level],
               s_color_reset,
               timestamp,
               filename,
               line);
        vprintf(format, args);
        printf("\n");
        fflush(stdout);
        va_end(args);
    }
    // File output
    if ((s_log_state.output_flags & TF_LOG_OUTPUT_FILE) && s_log_state.log_file) {
        va_list args;
        va_start(args, format);
        fprintf(s_log_state.log_file, "[%s] [%s] %s:%u: ",
                s_log_level_strings[level],
                timestamp,
                filename,
                line);
        vfprintf(s_log_state.log_file, format, args);
        fprintf(s_log_state.log_file, "\n");
        fflush(s_log_state.log_file);
        va_end(args);
    }
}
