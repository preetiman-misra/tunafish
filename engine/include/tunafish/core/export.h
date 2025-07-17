//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Platform detection (64-bit only)
#if defined(_WIN64)
#define TF_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define TF_PLATFORM_MACOS
#elif defined(__linux__)
#define TF_PLATFORM_LINUX
#else
#define TF_PLATFORM_UNKNOWN
#endif

// Compiler detection
#if defined(_MSC_VER)
#define TF_COMPILER_MSVC
#elif defined(__clang__)
#define TF_COMPILER_CLANG
#elif defined(__GNUC__)
#define TF_COMPILER_GCC
#endif

// API export/import
#ifdef TF_PLATFORM_WINDOWS
#ifdef TF_EXPORTS
#define TF_API __declspec(dllexport)
#else
#define TF_API __declspec(dllimport)
#endif
#else
#if defined(TF_COMPILER_GCC) || defined(TF_COMPILER_CLANG)
#define TF_API __attribute__((visibility("default")))
#else
#define TF_API
#endif
#endif

// Function attributes
#define TF_INLINE static inline

// Debug/release (renamed to avoid conflict with logging)
#ifdef NDEBUG
#define TF_IS_DEBUG 0
#else
#define TF_IS_DEBUG 1
#endif

#ifdef __cplusplus
}
#endif
