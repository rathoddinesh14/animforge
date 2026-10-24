# Samples — require a GPU/display to run; always compile when AGA_BUILD_SAMPLES=ON.

add_executable(aga_sample_00_hello_triangle
    00_hello_triangle/main.cpp
)

target_link_libraries(aga_sample_00_hello_triangle
    PRIVATE
        aga_core
)

target_include_directories(aga_sample_00_hello_triangle
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
