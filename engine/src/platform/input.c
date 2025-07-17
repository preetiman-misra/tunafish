//
// Created by Preetiman Misra on 17/07/25.
//
#include "tunafish/platform/input.h"
#include "tunafish/platform/window.h"
#include "tunafish/core/log.h"
#include "tunafish/core/memory.h"
#include <GLFW/glfw3.h>
#include <string.h>

// Input state structure
typedef struct {
    b32 current[TF_MAX_KEYS];
    b32 previous[TF_MAX_KEYS];
} TF_KeyState;

typedef struct {
    b32 current[TF_MAX_MOUSE_BUTTONS];
    b32 previous[TF_MAX_MOUSE_BUTTONS];
} TF_MouseButtonState;

typedef struct {
    TF_MousePos current;
    TF_MousePos previous;
    TF_MousePos delta;
} TF_MousePositionState;

typedef struct {
    TF_ScrollOffset current;
    TF_ScrollOffset previous;
    TF_ScrollOffset delta;
} TF_ScrollState;

// Global input state
static struct {
    b32 initialized;
    TF_Window *window;
    struct GLFWwindow *glfw_window;

    // Input states
    TF_KeyState keys;
    TF_MouseButtonState mouse_buttons;
    TF_MousePositionState mouse_position;
    TF_ScrollState scroll;

    // Cursor state
    b32 cursor_visible;
    b32 cursor_locked;

    // Text input
    b32 text_input_enabled;
    char text_input_buffer[256];
    u32 text_input_length;

    // Frame tracking
    b32 frame_updated;
} s_input_state = {0};

// GLFW callback functions
static void tf_input_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void) window; // Unused
    (void) scancode; // Unused
    (void) mods; // Unused

    if (key >= 0 && key < TF_MAX_KEYS) {
        if (action == GLFW_PRESS) {
            s_input_state.keys.current[key] = TF_TRUE;
            TF_DEBUG_TRACE("Key %d pressed", key);
        } else if (action == GLFW_RELEASE) {
            s_input_state.keys.current[key] = TF_FALSE;
            TF_DEBUG_TRACE("Key %d released", key);
        }
        // Note: GLFW_REPEAT is handled by keeping the key pressed
    }
}

static void tf_input_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    (void) window; // Unused
    (void) mods; // Unused

    if (button >= 0 && button < TF_MAX_MOUSE_BUTTONS) {
        if (action == GLFW_PRESS) {
            s_input_state.mouse_buttons.current[button] = TF_TRUE;
            TF_DEBUG_TRACE("Mouse button %d pressed", button);
        } else if (action == GLFW_RELEASE) {
            s_input_state.mouse_buttons.current[button] = TF_FALSE;
            TF_DEBUG_TRACE("Mouse button %d released", button);
        }
    }
}

static void tf_input_cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    (void) window; // Unused

    s_input_state.mouse_position.current.x = xpos;
    s_input_state.mouse_position.current.y = ypos;
    TF_DEBUG_TRACE("Mouse position: (%.2f, %.2f)", xpos, ypos);
}

static void tf_input_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    (void) window; // Unused

    s_input_state.scroll.current.x += xoffset;
    s_input_state.scroll.current.y += yoffset;
    TF_DEBUG_TRACE("Scroll: (%.2f, %.2f)", xoffset, yoffset);
}

static void tf_input_char_callback(GLFWwindow *window, unsigned int codepoint) {
    (void) window; // Unused

    if (!s_input_state.text_input_enabled) {
        return;
    }

    // Convert Unicode codepoint to UTF-8 (ASCII only for simplicity)
    if (codepoint < 0x80) {
        if (s_input_state.text_input_length < sizeof(s_input_state.text_input_buffer) - 1) {
            s_input_state.text_input_buffer[s_input_state.text_input_length++] = (char) codepoint;
            s_input_state.text_input_buffer[s_input_state.text_input_length] = '\0';
            TF_DEBUG_TRACE("Text input: '%c'", (char)codepoint);
        }
    }
}

// Core input system API
TF_API void tf_input_init(void) {
    if (s_input_state.initialized) {
        TF_WARN("Input system already initialized");
        return;
    }

    TF_DEBUG("Initializing input system...");

    // Initialize all states to false/zero
    memset(&s_input_state.keys, 0, sizeof(s_input_state.keys));
    memset(&s_input_state.mouse_buttons, 0, sizeof(s_input_state.mouse_buttons));
    memset(&s_input_state.mouse_position, 0, sizeof(s_input_state.mouse_position));
    memset(&s_input_state.scroll, 0, sizeof(s_input_state.scroll));

    s_input_state.cursor_visible = TF_TRUE;
    s_input_state.cursor_locked = TF_FALSE;
    s_input_state.text_input_enabled = TF_FALSE;
    s_input_state.text_input_length = 0;
    s_input_state.text_input_buffer[0] = '\0';
    s_input_state.frame_updated = TF_FALSE;

    s_input_state.window = TF_NULL;
    s_input_state.glfw_window = TF_NULL;

    s_input_state.initialized = TF_TRUE;
    TF_INFO("Input system initialized");
}

TF_API void tf_input_shutdown(void) {
    if (!s_input_state.initialized) {
        TF_WARN("Input system not initialized");
        return;
    }

    TF_DEBUG("Shutting down input system...");

    // Clear callbacks if we have a window
    if (s_input_state.glfw_window) {
        glfwSetKeyCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetMouseButtonCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetCursorPosCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetScrollCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetCharCallback(s_input_state.glfw_window, TF_NULL);
        TF_DEBUG("Input callbacks cleared");
    }

    s_input_state.initialized = TF_FALSE;
    TF_INFO("Input system shutdown");
}

TF_API void tf_input_update(void) {
    if (!s_input_state.initialized) {
        TF_WARN("Input system not initialized");
        return;
    }

    // Copy current state to previous state
    memcpy(s_input_state.keys.previous, s_input_state.keys.current, sizeof(s_input_state.keys.current));
    memcpy(s_input_state.mouse_buttons.previous, s_input_state.mouse_buttons.current,
           sizeof(s_input_state.mouse_buttons.current));

    // Update mouse delta
    s_input_state.mouse_position.delta.x = s_input_state.mouse_position.current.x - s_input_state.mouse_position.
                                           previous.x;
    s_input_state.mouse_position.delta.y = s_input_state.mouse_position.current.y - s_input_state.mouse_position.
                                           previous.y;
    s_input_state.mouse_position.previous = s_input_state.mouse_position.current;

    // Update scroll delta
    s_input_state.scroll.delta.x = s_input_state.scroll.current.x - s_input_state.scroll.previous.x;
    s_input_state.scroll.delta.y = s_input_state.scroll.current.y - s_input_state.scroll.previous.y;
    s_input_state.scroll.previous = s_input_state.scroll.current;

    s_input_state.frame_updated = TF_TRUE;

    TF_DEBUG_TRACE("Input updated: mouse delta (%.2f, %.2f), scroll delta (%.2f, %.2f)",
                   s_input_state.mouse_position.delta.x, s_input_state.mouse_position.delta.y,
                   s_input_state.scroll.delta.x, s_input_state.scroll.delta.y)    ;
}

TF_API void tf_input_set_window(TF_Window *window) {
    if (!s_input_state.initialized) {
        TF_WARN("Input system not initialized");
        return;
    }

    if (!window) {
        TF_WARN("Cannot set null window for input");
        return;
    }

    TF_DEBUG("Setting input window");

    // Clear previous callbacks if we had a window
    if (s_input_state.glfw_window) {
        glfwSetKeyCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetMouseButtonCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetCursorPosCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetScrollCallback(s_input_state.glfw_window, TF_NULL);
        glfwSetCharCallback(s_input_state.glfw_window, TF_NULL);
    }

    s_input_state.window = window;
    s_input_state.glfw_window = tf_window_get_glfw_window(window);

    if (s_input_state.glfw_window) {
        // Set up GLFW callbacks
        glfwSetKeyCallback(s_input_state.glfw_window, tf_input_key_callback);
        glfwSetMouseButtonCallback(s_input_state.glfw_window, tf_input_mouse_button_callback);
        glfwSetCursorPosCallback(s_input_state.glfw_window, tf_input_cursor_position_callback);
        glfwSetScrollCallback(s_input_state.glfw_window, tf_input_scroll_callback);
        glfwSetCharCallback(s_input_state.glfw_window, tf_input_char_callback);

        // Get initial mouse position
        double xpos, ypos;
        glfwGetCursorPos(s_input_state.glfw_window, &xpos, &ypos);
        s_input_state.mouse_position.current.x = xpos;
        s_input_state.mouse_position.current.y = ypos;
        s_input_state.mouse_position.previous = s_input_state.mouse_position.current;

        TF_INFO("Input callbacks set for window");
    } else {
        TF_ERROR("Failed to get GLFW window handle");
    }
}

// Keyboard input
TF_API b32 tf_input_is_key_pressed(TF_KeyCode key) {
    if (!s_input_state.initialized || key < 0 || key >= TF_MAX_KEYS) {
        return TF_FALSE;
    }
    return s_input_state.keys.current[key];
}

TF_API b32 tf_input_is_key_held(TF_KeyCode key) {
    if (!s_input_state.initialized || key < 0 || key >= TF_MAX_KEYS) {
        return TF_FALSE;
    }
    // Key is held if it was pressed last frame AND is still pressed this frame
    return s_input_state.keys.previous[key] && s_input_state.keys.current[key];
}

TF_API b32 tf_input_was_key_just_pressed(TF_KeyCode key) {
    if (!s_input_state.initialized || key < 0 || key >= TF_MAX_KEYS) {
        return TF_FALSE;
    }
    return s_input_state.keys.current[key] && !s_input_state.keys.previous[key];
}

TF_API b32 tf_input_was_key_just_released(TF_KeyCode key) {
    if (!s_input_state.initialized || key < 0 || key >= TF_MAX_KEYS) {
        return TF_FALSE;
    }
    return !s_input_state.keys.current[key] && s_input_state.keys.previous[key];
}

// Mouse button input
TF_API b32 tf_input_is_mouse_button_pressed(TF_MouseButton button) {
    if (!s_input_state.initialized || button < 0 || button >= TF_MAX_MOUSE_BUTTONS) {
        return TF_FALSE;
    }
    return s_input_state.mouse_buttons.current[button];
}

TF_API b32 tf_input_is_mouse_button_held(TF_MouseButton button) {
    if (!s_input_state.initialized || button < 0 || button >= TF_MAX_MOUSE_BUTTONS) {
        return TF_FALSE;
    }
    // Button is held if it was pressed last frame AND is still pressed this frame
    return s_input_state.mouse_buttons.previous[button] && s_input_state.mouse_buttons.current[button];
}

TF_API b32 tf_input_was_mouse_button_just_pressed(TF_MouseButton button) {
    if (!s_input_state.initialized || button < 0 || button >= TF_MAX_MOUSE_BUTTONS) {
        return TF_FALSE;
    }
    return s_input_state.mouse_buttons.current[button] && !s_input_state.mouse_buttons.previous[button];
}

TF_API b32 tf_input_was_mouse_button_just_released(TF_MouseButton button) {
    if (!s_input_state.initialized || button < 0 || button >= TF_MAX_MOUSE_BUTTONS) {
        return TF_FALSE;
    }
    return !s_input_state.mouse_buttons.current[button] && s_input_state.mouse_buttons.previous[button];
}

// Mouse position and movement
TF_API TF_MousePos tf_input_get_mouse_position(void) {
    if (!s_input_state.initialized) {
        return (TF_MousePos){0.0, 0.0};
    }
    return s_input_state.mouse_position.current;
}

TF_API TF_MousePos tf_input_get_mouse_delta(void) {
    if (!s_input_state.initialized) {
        return (TF_MousePos){0.0, 0.0};
    }
    return s_input_state.mouse_position.delta;
}

TF_API void tf_input_set_mouse_position(f64 x, f64 y) {
    if (!s_input_state.initialized || !s_input_state.glfw_window) {
        TF_WARN("Cannot set mouse position: input system not ready");
        return;
    }

    glfwSetCursorPos(s_input_state.glfw_window, x, y);
    s_input_state.mouse_position.current.x = x;
    s_input_state.mouse_position.current.y = y;
    TF_DEBUG("Mouse position set to (%.2f, %.2f)", x, y);
}

// Mouse cursor control
TF_API void tf_input_set_cursor_visible(b32 visible) {
    if (!s_input_state.initialized || !s_input_state.glfw_window) {
        TF_WARN("Cannot set cursor visibility: input system not ready");
        return;
    }

    s_input_state.cursor_visible = visible;
    glfwSetInputMode(s_input_state.glfw_window, GLFW_CURSOR,
                     visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    TF_DEBUG("Cursor visibility set to %s", visible ? "visible" : "hidden");
}

TF_API void tf_input_set_cursor_locked(b32 locked) {
    if (!s_input_state.initialized || !s_input_state.glfw_window) {
        TF_WARN("Cannot set cursor lock: input system not ready");
        return;
    }

    s_input_state.cursor_locked = locked;
    glfwSetInputMode(s_input_state.glfw_window, GLFW_CURSOR,
                     locked
                         ? GLFW_CURSOR_DISABLED
                         : (s_input_state.cursor_visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN));
    TF_DEBUG("Cursor lock set to %s", locked ? "locked" : "unlocked");
}

TF_API b32 tf_input_is_cursor_visible(void) {
    return s_input_state.initialized ? s_input_state.cursor_visible : TF_TRUE;
}

TF_API b32 tf_input_is_cursor_locked(void) {
    return s_input_state.initialized ? s_input_state.cursor_locked : TF_FALSE;
}

// Scroll input
TF_API TF_ScrollOffset tf_input_get_scroll_offset(void) {
    if (!s_input_state.initialized) {
        return (TF_ScrollOffset){0.0, 0.0};
    }
    return s_input_state.scroll.current;
}

TF_API TF_ScrollOffset tf_input_get_scroll_delta(void) {
    if (!s_input_state.initialized) {
        return (TF_ScrollOffset){0.0, 0.0};
    }
    return s_input_state.scroll.delta;
}

// Text input (for UI)
TF_API void tf_input_enable_text_input(b32 enabled) {
    if (!s_input_state.initialized) {
        TF_WARN("Cannot enable text input: input system not ready");
        return;
    }

    s_input_state.text_input_enabled = enabled;
    if (!enabled) {
        tf_input_clear_text_input();
    }
    TF_DEBUG("Text input %s", enabled ? "enabled" : "disabled");
}

TF_API b32 tf_input_is_text_input_enabled(void) {
    return s_input_state.initialized ? s_input_state.text_input_enabled : TF_FALSE;
}

TF_API const char *tf_input_get_text_input(void) {
    if (!s_input_state.initialized) {
        return "";
    }
    return s_input_state.text_input_buffer;
}

TF_API void tf_input_clear_text_input(void) {
    if (!s_input_state.initialized) {
        return;
    }

    s_input_state.text_input_length = 0;
    s_input_state.text_input_buffer[0] = '\0';
    TF_DEBUG_TRACE("Text input buffer cleared");
}

// Utility functions
TF_API const char *tf_input_get_key_name(TF_KeyCode key) {
    switch (key) {
        case TF_KEY_SPACE: return "Space";
        case TF_KEY_APOSTROPHE: return "'";
        case TF_KEY_COMMA: return ",";
        case TF_KEY_MINUS: return "-";
        case TF_KEY_PERIOD: return ".";
        case TF_KEY_SLASH: return "/";
        case TF_KEY_0: return "0";
        case TF_KEY_1: return "1";
        case TF_KEY_2: return "2";
        case TF_KEY_3: return "3";
        case TF_KEY_4: return "4";
        case TF_KEY_5: return "5";
        case TF_KEY_6: return "6";
        case TF_KEY_7: return "7";
        case TF_KEY_8: return "8";
        case TF_KEY_9: return "9";
        case TF_KEY_SEMICOLON: return ";";
        case TF_KEY_EQUAL: return "=";
        case TF_KEY_A: return "A";
        case TF_KEY_B: return "B";
        case TF_KEY_C: return "C";
        case TF_KEY_D: return "D";
        case TF_KEY_E: return "E";
        case TF_KEY_F: return "F";
        case TF_KEY_G: return "G";
        case TF_KEY_H: return "H";
        case TF_KEY_I: return "I";
        case TF_KEY_J: return "J";
        case TF_KEY_K: return "K";
        case TF_KEY_L: return "L";
        case TF_KEY_M: return "M";
        case TF_KEY_N: return "N";
        case TF_KEY_O: return "O";
        case TF_KEY_P: return "P";
        case TF_KEY_Q: return "Q";
        case TF_KEY_R: return "R";
        case TF_KEY_S: return "S";
        case TF_KEY_T: return "T";
        case TF_KEY_U: return "U";
        case TF_KEY_V: return "V";
        case TF_KEY_W: return "W";
        case TF_KEY_X: return "X";
        case TF_KEY_Y: return "Y";
        case TF_KEY_Z: return "Z";
        case TF_KEY_LEFT_BRACKET: return "[";
        case TF_KEY_BACKSLASH: return "\\";
        case TF_KEY_RIGHT_BRACKET: return "]";
        case TF_KEY_GRAVE_ACCENT: return "`";
        case TF_KEY_ESCAPE: return "Escape";
        case TF_KEY_ENTER: return "Enter";
        case TF_KEY_TAB: return "Tab";
        case TF_KEY_BACKSPACE: return "Backspace";
        case TF_KEY_INSERT: return "Insert";
        case TF_KEY_DELETE: return "Delete";
        case TF_KEY_RIGHT: return "Right";
        case TF_KEY_LEFT: return "Left";
        case TF_KEY_DOWN: return "Down";
        case TF_KEY_UP: return "Up";
        case TF_KEY_PAGE_UP: return "Page Up";
        case TF_KEY_PAGE_DOWN: return "Page Down";
        case TF_KEY_HOME: return "Home";
        case TF_KEY_END: return "End";
        case TF_KEY_CAPS_LOCK: return "Caps Lock";
        case TF_KEY_SCROLL_LOCK: return "Scroll Lock";
        case TF_KEY_NUM_LOCK: return "Num Lock";
        case TF_KEY_PRINT_SCREEN: return "Print Screen";
        case TF_KEY_PAUSE: return "Pause";
        case TF_KEY_F1: return "F1";
        case TF_KEY_F2: return "F2";
        case TF_KEY_F3: return "F3";
        case TF_KEY_F4: return "F4";
        case TF_KEY_F5: return "F5";
        case TF_KEY_F6: return "F6";
        case TF_KEY_F7: return "F7";
        case TF_KEY_F8: return "F8";
        case TF_KEY_F9: return "F9";
        case TF_KEY_F10: return "F10";
        case TF_KEY_F11: return "F11";
        case TF_KEY_F12: return "F12";
        case TF_KEY_LEFT_SHIFT: return "Left Shift";
        case TF_KEY_LEFT_CONTROL: return "Left Ctrl";
        case TF_KEY_LEFT_ALT: return "Left Alt";
        case TF_KEY_LEFT_SUPER: return "Left Super";
        case TF_KEY_RIGHT_SHIFT: return "Right Shift";
        case TF_KEY_RIGHT_CONTROL: return "Right Ctrl";
        case TF_KEY_RIGHT_ALT: return "Right Alt";
        case TF_KEY_RIGHT_SUPER: return "Right Super";
        case TF_KEY_MENU: return "Menu";
        default: return "Unknown";
    }
}

TF_API b32 tf_input_is_key_valid(TF_KeyCode key) {
    return key >= 0 && key < TF_MAX_KEYS && key != TF_KEY_UNKNOWN;
}

TF_API b32 tf_input_is_mouse_button_valid(TF_MouseButton button) {
    return button >= 0 && button < TF_MAX_MOUSE_BUTTONS;
}
