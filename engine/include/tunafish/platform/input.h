//
// Created by Preetiman Misra on 17/07/25.
//
#pragma once

#include "tunafish/core/types.h"
#include "tunafish/core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct TF_Window TF_Window;

// Maximum number of keys and mouse buttons
#define TF_MAX_KEYS 512
#define TF_MAX_MOUSE_BUTTONS 8

// Key codes (GLFW compatible)
typedef enum {
    // Printable keys
    TF_KEY_SPACE = 32,
    TF_KEY_APOSTROPHE = 39,
    TF_KEY_COMMA = 44,
    TF_KEY_MINUS = 45,
    TF_KEY_PERIOD = 46,
    TF_KEY_SLASH = 47,
    TF_KEY_0 = 48,
    TF_KEY_1 = 49,
    TF_KEY_2 = 50,
    TF_KEY_3 = 51,
    TF_KEY_4 = 52,
    TF_KEY_5 = 53,
    TF_KEY_6 = 54,
    TF_KEY_7 = 55,
    TF_KEY_8 = 56,
    TF_KEY_9 = 57,
    TF_KEY_SEMICOLON = 59,
    TF_KEY_EQUAL = 61,
    TF_KEY_A = 65,
    TF_KEY_B = 66,
    TF_KEY_C = 67,
    TF_KEY_D = 68,
    TF_KEY_E = 69,
    TF_KEY_F = 70,
    TF_KEY_G = 71,
    TF_KEY_H = 72,
    TF_KEY_I = 73,
    TF_KEY_J = 74,
    TF_KEY_K = 75,
    TF_KEY_L = 76,
    TF_KEY_M = 77,
    TF_KEY_N = 78,
    TF_KEY_O = 79,
    TF_KEY_P = 80,
    TF_KEY_Q = 81,
    TF_KEY_R = 82,
    TF_KEY_S = 83,
    TF_KEY_T = 84,
    TF_KEY_U = 85,
    TF_KEY_V = 86,
    TF_KEY_W = 87,
    TF_KEY_X = 88,
    TF_KEY_Y = 89,
    TF_KEY_Z = 90,
    TF_KEY_LEFT_BRACKET = 91,
    TF_KEY_BACKSLASH = 92,
    TF_KEY_RIGHT_BRACKET = 93,
    TF_KEY_GRAVE_ACCENT = 96,

    // Function keys
    TF_KEY_ESCAPE = 256,
    TF_KEY_ENTER = 257,
    TF_KEY_TAB = 258,
    TF_KEY_BACKSPACE = 259,
    TF_KEY_INSERT = 260,
    TF_KEY_DELETE = 261,
    TF_KEY_RIGHT = 262,
    TF_KEY_LEFT = 263,
    TF_KEY_DOWN = 264,
    TF_KEY_UP = 265,
    TF_KEY_PAGE_UP = 266,
    TF_KEY_PAGE_DOWN = 267,
    TF_KEY_HOME = 268,
    TF_KEY_END = 269,
    TF_KEY_CAPS_LOCK = 280,
    TF_KEY_SCROLL_LOCK = 281,
    TF_KEY_NUM_LOCK = 282,
    TF_KEY_PRINT_SCREEN = 283,
    TF_KEY_PAUSE = 284,
    TF_KEY_F1 = 290,
    TF_KEY_F2 = 291,
    TF_KEY_F3 = 292,
    TF_KEY_F4 = 293,
    TF_KEY_F5 = 294,
    TF_KEY_F6 = 295,
    TF_KEY_F7 = 296,
    TF_KEY_F8 = 297,
    TF_KEY_F9 = 298,
    TF_KEY_F10 = 299,
    TF_KEY_F11 = 300,
    TF_KEY_F12 = 301,

    // Keypad
    TF_KEY_KP_0 = 320,
    TF_KEY_KP_1 = 321,
    TF_KEY_KP_2 = 322,
    TF_KEY_KP_3 = 323,
    TF_KEY_KP_4 = 324,
    TF_KEY_KP_5 = 325,
    TF_KEY_KP_6 = 326,
    TF_KEY_KP_7 = 327,
    TF_KEY_KP_8 = 328,
    TF_KEY_KP_9 = 329,
    TF_KEY_KP_DECIMAL = 330,
    TF_KEY_KP_DIVIDE = 331,
    TF_KEY_KP_MULTIPLY = 332,
    TF_KEY_KP_SUBTRACT = 333,
    TF_KEY_KP_ADD = 334,
    TF_KEY_KP_ENTER = 335,
    TF_KEY_KP_EQUAL = 336,

    // Modifiers
    TF_KEY_LEFT_SHIFT = 340,
    TF_KEY_LEFT_CONTROL = 341,
    TF_KEY_LEFT_ALT = 342,
    TF_KEY_LEFT_SUPER = 343,
    TF_KEY_RIGHT_SHIFT = 344,
    TF_KEY_RIGHT_CONTROL = 345,
    TF_KEY_RIGHT_ALT = 346,
    TF_KEY_RIGHT_SUPER = 347,
    TF_KEY_MENU = 348,

    TF_KEY_UNKNOWN = -1
} TF_KeyCode;

// Mouse button codes
typedef enum {
    TF_MOUSE_BUTTON_LEFT = 0,
    TF_MOUSE_BUTTON_RIGHT = 1,
    TF_MOUSE_BUTTON_MIDDLE = 2,
    TF_MOUSE_BUTTON_4 = 3,
    TF_MOUSE_BUTTON_5 = 4,
    TF_MOUSE_BUTTON_6 = 5,
    TF_MOUSE_BUTTON_7 = 6,
    TF_MOUSE_BUTTON_8 = 7
} TF_MouseButton;

// Mouse position
typedef struct {
    f64 x;
    f64 y;
} TF_MousePos;

// Scroll offset
typedef struct {
    f64 x;
    f64 y;
} TF_ScrollOffset;

// Core input system API
TF_API void tf_input_init(void);

TF_API void tf_input_shutdown(void);

TF_API void tf_input_update(void);

TF_API void tf_input_set_window(TF_Window *window);

// Keyboard input
TF_API b32 tf_input_is_key_pressed(TF_KeyCode key);

TF_API b32 tf_input_is_key_held(TF_KeyCode key);

TF_API b32 tf_input_was_key_just_pressed(TF_KeyCode key);

TF_API b32 tf_input_was_key_just_released(TF_KeyCode key);

// Mouse button input
TF_API b32 tf_input_is_mouse_button_pressed(TF_MouseButton button);

TF_API b32 tf_input_is_mouse_button_held(TF_MouseButton button);

TF_API b32 tf_input_was_mouse_button_just_pressed(TF_MouseButton button);

TF_API b32 tf_input_was_mouse_button_just_released(TF_MouseButton button);

// Mouse position and movement
TF_API TF_MousePos tf_input_get_mouse_position(void);

TF_API TF_MousePos tf_input_get_mouse_delta(void);

TF_API void tf_input_set_mouse_position(f64 x, f64 y);

// Mouse cursor control
TF_API void tf_input_set_cursor_visible(b32 visible);

TF_API void tf_input_set_cursor_locked(b32 locked);

TF_API b32 tf_input_is_cursor_visible(void);

TF_API b32 tf_input_is_cursor_locked(void);

// Scroll input
TF_API TF_ScrollOffset tf_input_get_scroll_offset(void);

TF_API TF_ScrollOffset tf_input_get_scroll_delta(void);

// Text input (for UI)
TF_API void tf_input_enable_text_input(b32 enabled);

TF_API b32 tf_input_is_text_input_enabled(void);

TF_API const char *tf_input_get_text_input(void);

TF_API void tf_input_clear_text_input(void);

// Utility functions
TF_API const char *tf_input_get_key_name(TF_KeyCode key);

TF_API b32 tf_input_is_key_valid(TF_KeyCode key);

TF_API b32 tf_input_is_mouse_button_valid(TF_MouseButton button);

#ifdef __cplusplus
}
#endif
