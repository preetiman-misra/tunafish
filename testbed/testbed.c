//
// Created by Preetiman Misra on 17/07/25.
//
#include <tunafish/tunafish.h>
#include <stdio.h>
#include <_stdlib.h>

int main(void) {
    printf("Tunafish Engine Test\n");
    printf("====================\n");

    TF_Engine *engine = tf_engine_create();
    if (!engine) {
        printf("[ERROR] Failed to create TF engine\n");
        return EXIT_FAILURE;
    }
    printf("✅Engine created\n");

    // Initialize
    if (!tf_engine_initialize(engine)) {
        printf("[ERROR] Failed to initialize engine\n");
        tf_engine_destroy(engine);
        return EXIT_FAILURE;
    }

    // Check if running
    if (tf_engine_is_running(engine)) {
        printf("✅Engine is running\n");
    }

    // Run a few frames
    printf("Running 5 frames...\n");
    for (int i = 0; i < 5; i++) {
        tf_engine_run_frame(engine);
        printf("  Frame %d completed\n", i + 1);
    }

    // Shutdown
    tf_engine_shutdown(engine);
    printf("✅Engine shutdown\n");

    tf_engine_destroy(engine);
    printf("✅Engine destroyed\n");

    printf("\n Test completed successfully!\n");
    return EXIT_SUCCESS;
}
