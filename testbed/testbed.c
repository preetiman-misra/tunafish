//
// Created by Preetiman Misra on 17/07/25.
//
#include <tunafish/tunafish.h>
#include <stdio.h>

int main(void) {
   printf("Tunafish Engine Test\n");
   printf("===================\n");
   // Test engine creation
   TF_Engine *engine = tf_engine_create();
   if (!engine) {
      printf("ERROR: Failed to create engine\n");
      return -1;
   }
   printf("✓ Engine created\n");
   // Test window creation
   const TF_WindowConfig window_config = {
      .title = "Tunafish Test Window",
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
   printf("✓ Window created (800x600)\n");
   // Initialize engine
   if (!tf_engine_initialize(engine)) {
      printf("ERROR: Failed to initialize engine\n");
      tf_window_destroy(window);
      tf_engine_destroy(engine);
      return -1;
   }
   printf("✓ Engine initialized\n");
   // Test window size
   u32 width, height;
   tf_window_get_size(window, &width, &height);
   printf("✓ Window size: %u x %u\n", width, height);
   // Run main loop for a few seconds
   printf("Running window for 5 seconds...\n");
   int frame_count = 0;
   while (!tf_window_should_close(window) && frame_count < 300) {
      // ~3 seconds at 60fps
      tf_window_poll_events(window);
      tf_engine_run_frame(engine);
      tf_window_swap_buffers(window);
      frame_count++;
   }
   printf("✓ Rendered %d frames\n", frame_count);
   // Cleanup
   tf_engine_shutdown(engine);
   printf("✓ Engine shutdown\n");
   tf_window_destroy(window);
   printf("✓ Window destroyed\n");

   tf_engine_destroy(engine);
   printf("✓ Engine destroyed\n");

   printf("\nTest completed successfully!\n");
   return 0;
}
