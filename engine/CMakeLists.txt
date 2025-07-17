# Vendor
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

add_library(tunafish_engine SHARED
        src/tunafish.c
        src/core/log.c
        src/core/math.c
        src/core/time.c
        src/platform/window.c
        src/core/memory.c
)

target_include_directories(tunafish_engine
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE src/ vendor/glfw/include
)

target_link_libraries(tunafish_engine PRIVATE glfw)

set_property(TARGET tunafish_engine PROPERTY C_STANDARD 11)
set_property(TARGET tunafish_engine PROPERTY C_STANDARD_REQUIRED ON)

if (WIN32)
    target_compile_definitions(tunafish_engine PRIVATE TF_EXPORTS)
endif ()

set_target_properties(tunafish_engine PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
)

if (APPLE)
    set_target_properties(tunafish_engine PROPERTIES
            INSTALL_NAME_DIR "@rpath"
    )
elseif (UNIX AND NOT APPLE)
    target_link_libraries(tunafish_engine PRIVATE m)
endif ()

# Alias for easy clean linking
add_library(tunafish::engine ALIAS tunafish_engine)