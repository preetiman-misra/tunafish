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

int main(void) {
   printf("Tunafish Engine Test with Math Library\n");
   printf("=====================================\n");
   // Test engine creation
   TF_Engine *engine = tf_engine_create();
   if (!engine) {
      printf("ERROR: Failed to create engine\n");
      return -1;
   }
   // Test window creation
   TF_WindowConfig window_config = {
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
   // Initialize engine (this will start logging)
   if (!tf_engine_initialize(engine)) {
      printf("ERROR: Failed to initialize engine\n");
      tf_window_destroy(window);
      tf_engine_destroy(engine);
      return -1;
   }
   // Test math library
   test_math_library();
   // Now we can use logging
   TF_INFO("Starting testbed main loop");
   // Test window size
   u32 width, height;
   tf_window_get_size(window, &width, &height);
   TF_DEBUG("Window size: %u x %u", width, height);
   // Run main loop for a few seconds
   TF_INFO("Running window for 3 seconds...");
   int frame_count = 0;
   while (!tf_window_should_close(window) && frame_count < 180) {
      tf_window_poll_events(window);
      tf_engine_run_frame(engine);
      tf_window_swap_buffers(window);
      frame_count++;
      // Log every 60 frames (roughly every second)
      if (frame_count % 60 == 0) {
         TF_INFO("Rendered %d frames", frame_count);
      }
   }
   TF_INFO("Main loop completed with %d frames", frame_count);

   // Cleanup
   tf_engine_shutdown(engine);
   tf_window_destroy(window);
   tf_engine_destroy(engine);

   printf("\nTest completed - check the colored log output above!\n");
   return 0;
}
