//
// Created by Preetiman Misra on 17/07/25.
//
#include <tunafish/tunafish.h>
#include <stdio.h>

void test_math_library(void) {
    TF_INFO("Testing math library...");
    // Test vector operations
    TF_Vec3 v1 = tf_vec3_create(1.0f, 2.0f, 3.0f);
    TF_Vec3 v2 = tf_vec3_create(4.0f, 5.0f, 6.0f);
    TF_Vec3 v3 = tf_vec3_add(v1, v2);
    TF_DEBUG("Vector addition: (1,2,3) + (4,5,6) = (%.1f,%.1f,%.1f)", v3.x, v3.y, v3.z);

    // Test matrix operations
    TF_Mat4 identity = tf_mat4_identity();
    TF_Mat4 translation = tf_mat4_translate(tf_vec3_create(10.0f, 20.0f, 30.0f));
    TF_Mat4 result = tf_mat4_multiply(identity, translation);
    TF_DEBUG("Translation matrix [12] = %.1f (should be 10.0)", result.m[12]);
    TF_DEBUG("Translation matrix [13] = %.1f (should be 20.0)", result.m[13]);
    TF_DEBUG("Translation matrix [14] = %.1f (should be 30.0)", result.m[14]);

    // Test angle conversion
    f32 degrees = 90.0f;
    f32 radians = tf_radians(degrees);
    TF_DEBUG("90 degrees = %.3f radians", radians);

    TF_INFO("Math library tests completed");
}

void test_time_system(void) {
    TF_INFO("Testing time system...");

    // Test basic time functions
    f64 current_time = tf_time_get_current();
    f64 elapsed = tf_time_get_elapsed();
    u64 frame_count = tf_time_get_frame_count();
    TF_DEBUG("Current time: %.3f seconds", current_time);
    TF_DEBUG("Elapsed time: %.3f seconds", elapsed);
    TF_DEBUG("Frame count: %llu", frame_count);

    // Test target FPS
    tf_time_set_target_fps(60.0f);
    f32 target_fps = tf_time_get_target_fps();
    TF_DEBUG("Target FPS set to: %.1f", target_fps);

    // Test small sleep
    TF_DEBUG("Testing 10ms sleep...");
    f64 sleep_start = tf_time_get_current();
    tf_time_sleep(0.01f); // 10ms
    f64 sleep_end = tf_time_get_current();
    TF_DEBUG("Sleep took: %.1fms", (sleep_end - sleep_start) * 1000.0);

    TF_INFO("Time system tests completed");
}

void test_memory_system(void) {
    TF_INFO("Testing memory system...");

    // Test arena allocator
    TF_Arena *test_arena = tf_arena_create("test", TF_KILOBYTES(64));
    if (test_arena) {
        // Allocate some test data
        i32 *numbers = TF_ARENA_ALLOC_ARRAY(test_arena, i32, 100);
        if (numbers) {
            for (i32 i = 0; i < 100; i++) {
                numbers[i] = i * i;
            }
            TF_DEBUG("Arena test: allocated 100 integers, first=%d, last=%d",
                     numbers[0], numbers[99]);
        }

        TF_DEBUG("Arena usage: %llu/%llu bytes",
                 (unsigned long long)tf_arena_get_usage(test_arena),
                 (unsigned long long)test_arena->size);

        tf_arena_destroy(test_arena);
    }

    // Test pool allocator
    TF_Pool *test_pool = tf_pool_create("test_blocks", 64, 10);
    if (test_pool) {
        void *blocks[5];
        for (i32 i = 0; i < 5; i++) {
            blocks[i] = tf_pool_alloc(test_pool);
            TF_DEBUG("Pool allocated block %d: %p", i, blocks[i]);
        }

        // Free some blocks
        tf_pool_free(test_pool, blocks[2]);
        tf_pool_free(test_pool, blocks[4]);

        TF_DEBUG("Pool usage: %llu bytes, %llu blocks free",
                 (unsigned long long)tf_pool_get_usage(test_pool),
                 (unsigned long long)tf_pool_get_free_count(test_pool));

        tf_pool_destroy(test_pool);
    }

    // Test stack allocator with markers
    TF_Stack *test_stack = tf_stack_create("test_stack", TF_KILOBYTES(32));
    if (test_stack) {
        // Push marker and allocate
        tf_stack_push_marker(test_stack);
        void *data1 = tf_stack_alloc(test_stack, 1000);
        void *data2 = tf_stack_alloc(test_stack, 2000);
        TF_DEBUG("Stack allocated: %p, %p (usage: %llu bytes)",
                 data1, data2, (unsigned long long)tf_stack_get_usage(test_stack));

        // Pop marker to free both allocations
        tf_stack_pop_marker(test_stack);
        TF_DEBUG("Stack after pop: usage: %llu bytes",
                 (unsigned long long)tf_stack_get_usage(test_stack));

        tf_stack_destroy(test_stack);
    }

    // Print memory statistics
    tf_memory_print_stats();

    TF_INFO("Memory system tests completed");
}

void test_input_system(void) {
    TF_INFO("Testing input system...");

    // Test key name utility
    TF_DEBUG("Key names: WASD = '%s', '%s', '%s', '%s'",
             tf_input_get_key_name(TF_KEY_W),
             tf_input_get_key_name(TF_KEY_A),
             tf_input_get_key_name(TF_KEY_S),
             tf_input_get_key_name(TF_KEY_D));

    // Test key validation
    TF_DEBUG("Key validation: TF_KEY_A valid = %s, TF_KEY_UNKNOWN valid = %s",
             tf_input_is_key_valid(TF_KEY_A) ? "true" : "false",
             tf_input_is_key_valid(TF_KEY_UNKNOWN) ? "true" : "false");

    // Test mouse button validation
    TF_DEBUG("Mouse button validation: LEFT valid = %s, button 99 valid = %s",
             tf_input_is_mouse_button_valid(TF_MOUSE_BUTTON_LEFT) ? "true" : "false",
             tf_input_is_mouse_button_valid(99) ? "true" : "false");

    // Test initial cursor state
    TF_DEBUG("Initial cursor state: visible = %s, locked = %s",
             tf_input_is_cursor_visible() ? "true" : "false",
             tf_input_is_cursor_locked() ? "true" : "false");

    // Test text input state
    TF_DEBUG("Text input enabled: %s",
             tf_input_is_text_input_enabled() ? "true" : "false");

    TF_INFO("Input system tests completed");
}

void test_input_interactive(TF_Window *window) {
    TF_INFO("Starting interactive input test...");
    TF_INFO("Instructions:");
    TF_INFO("  - Press WASD keys to test keyboard input");
    TF_INFO("  - Click mouse buttons to test mouse input");
    TF_INFO("  - Move mouse to test position tracking");
    TF_INFO("  - Scroll to test scroll wheel");
    TF_INFO("  - Press ESCAPE to end test early");
    TF_INFO("  - Test will run for 10 seconds");

    f64 test_start = tf_time_get_current();
    f64 last_report = test_start;

    while (tf_time_get_current() - test_start < 10.0) {
        tf_window_poll_events(window);
        tf_input_update();

        // Check for escape key to exit early
        if (tf_input_was_key_just_pressed(TF_KEY_ESCAPE)) {
            TF_INFO("Escape pressed - ending input test early");
            break;
        }

        // Report input every 0.5 seconds
        f64 current_time = tf_time_get_current();
        if (current_time - last_report >= 0.5) {
            // Check WASD keys
            b32 w_pressed = tf_input_is_key_pressed(TF_KEY_W);
            b32 a_pressed = tf_input_is_key_pressed(TF_KEY_A);
            b32 s_pressed = tf_input_is_key_pressed(TF_KEY_S);
            b32 d_pressed = tf_input_is_key_pressed(TF_KEY_D);

            if (w_pressed || a_pressed || s_pressed || d_pressed) {
                TF_INFO("Keys pressed: %s%s%s%s",
                        w_pressed ? "W " : "",
                        a_pressed ? "A " : "",
                        s_pressed ? "S " : "",
                        d_pressed ? "D " : "");
            }

            // Check mouse buttons
            b32 left_pressed = tf_input_is_mouse_button_pressed(TF_MOUSE_BUTTON_LEFT);
            b32 right_pressed = tf_input_is_mouse_button_pressed(TF_MOUSE_BUTTON_RIGHT);
            b32 middle_pressed = tf_input_is_mouse_button_pressed(TF_MOUSE_BUTTON_MIDDLE);

            if (left_pressed || right_pressed || middle_pressed) {
                TF_INFO("Mouse buttons pressed: %s%s%s",
                        left_pressed ? "LEFT " : "",
                        right_pressed ? "RIGHT " : "",
                        middle_pressed ? "MIDDLE " : "");
            }

            // Check mouse position and delta
            TF_MousePos pos = tf_input_get_mouse_position();
            TF_MousePos delta = tf_input_get_mouse_delta();
            TF_DEBUG("Mouse: pos(%.1f, %.1f), delta(%.2f, %.2f)",
                     pos.x, pos.y, delta.x, delta.y);

            // Check scroll
            TF_ScrollOffset scroll_delta = tf_input_get_scroll_delta();
            if (scroll_delta.x != 0.0 || scroll_delta.y != 0.0) {
                TF_INFO("Scroll delta: (%.2f, %.2f)", scroll_delta.x, scroll_delta.y);
            }

            last_report = current_time;
        }

        // Small sleep to prevent busy waiting
        tf_time_sleep(0.001f);
    }

    TF_INFO("Interactive input test completed");
}

int main(void) {
    printf("Tunafish Engine Test with Input System\n");
    printf("======================================\n");

    // Test engine creation
    TF_Engine *engine = tf_engine_create();
    if (!engine) {
        printf("ERROR: Failed to create engine\n");
        return -1;
    }

    // Test window creation
    TF_WindowConfig window_config = {
        .title = "Tunafish Input Test Window",
        .width = 800,
        .height = 600,
        .resizable = TF_TRUE,
        .fullscreen = TF_FALSE
    };
    TF_Window *window = tf_window_create(&window_config);
    if (!window) {
        printf("ERROR: Failed to create window\n");
        tf_engine_destroy(engine);
        return -1;
    }

    // Initialize engine (this will start logging, memory, time, and input systems)
    if (!tf_engine_initialize(engine)) {
        printf("ERROR: Failed to initialize engine\n");
        tf_window_destroy(window);
        tf_engine_destroy(engine);
        return -1;
    }

    // Initialize input system and set window
    tf_input_init();
    tf_input_set_window(window);

    // Test all systems
    test_math_library();
    test_time_system();
    test_memory_system();
    test_input_system();

    // Interactive input testing
    test_input_interactive(window);

    // Now run the normal main loop
    TF_INFO("Starting normal main loop...");
    int frame_count = 0;
    f64 last_fps_report = tf_time_get_current();

    while (!tf_window_should_close(window) && frame_count < 300) {
        tf_window_poll_events(window);
        tf_input_update(); // Update input each frame
        tf_engine_run_frame(engine);
        tf_window_swap_buffers(window);
        frame_count++;

        // Check for escape to exit
        if (tf_input_was_key_just_pressed(TF_KEY_ESCAPE)) {
            TF_INFO("Escape pressed - exiting main loop");
            break;
        }

        // Report timing info every second
        f64 current_time = tf_time_get_current();
        if (current_time - last_fps_report >= 1.0) {
            f32 fps = tf_time_get_fps();
            f32 delta = tf_time_get_delta();

            // Show input state in FPS reports
            TF_MousePos mouse_pos = tf_input_get_mouse_position();
            TF_INFO("Frame %d - FPS: %.1f, Delta: %.3fms, Mouse: (%.0f,%.0f)",
                    frame_count, fps, delta * 1000.0f, mouse_pos.x, mouse_pos.y);
            last_fps_report = current_time;
        }
    }

    // Final timing report
    f64 total_elapsed = tf_time_get_elapsed();
    u64 total_frames = tf_time_get_frame_count();
    f32 avg_fps = (f32) total_frames / (f32) total_elapsed;
    TF_INFO("Main loop completed:");
    TF_INFO("  Total frames: %llu", total_frames);
    TF_INFO("  Total time: %.3f seconds", total_elapsed);
    TF_INFO("  Average FPS: %.1f", avg_fps);

    // Cleanup
    tf_input_shutdown();
    tf_engine_shutdown(engine);
    tf_window_destroy(window);
    tf_engine_destroy(engine);

    printf("\nTest completed - check the detailed output above!\n");
    return 0;
}
