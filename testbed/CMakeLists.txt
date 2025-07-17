add_executable(testbed testbed.c)

target_link_libraries(testbed PRIVATE tunafish::engine)