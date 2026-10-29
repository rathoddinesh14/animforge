# Educational primitive samples (GPU/display required to run).

function(aga_add_gl_sample name dir)
    add_executable(${name} ${dir}/main.cpp)
    target_link_libraries(${name} PRIVATE aga_core)
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/samples
    )
endfunction()

aga_add_gl_sample(aga_sample_00_gl_triangle 00_gl_triangle)
aga_add_gl_sample(aga_sample_01_gl_line 01_gl_line)
aga_add_gl_sample(aga_sample_02_gl_points 02_gl_points)

if(AGA_USE_VULKAN)
    find_package(Vulkan QUIET)
    if(Vulkan_FOUND)
        function(aga_add_vk_sample name dir)
            add_executable(${name} ${dir}/main.cpp)
            target_link_libraries(${name} PRIVATE aga_core Vulkan::Vulkan glfw)
            target_include_directories(${name} PRIVATE
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_SOURCE_DIR}/samples
            )
            target_compile_definitions(${name} PRIVATE AGA_USE_VULKAN=1)
        endfunction()
        aga_add_vk_sample(aga_sample_10_vk_triangle 10_vk_triangle)
        message(STATUS "Vulkan samples: enabled")
    else()
        message(STATUS "Vulkan samples: skipped (Vulkan package not found)")
    endif()
endif()
