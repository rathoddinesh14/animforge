function(aga_add_gl_sample name dir)
    add_executable(aga_sample_${name} ${dir}/main.cpp)

    target_include_directories(aga_sample_${name} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_link_libraries(aga_sample_${name} PRIVATE aga_core)
endfunction()

aga_add_gl_sample(00_gl_triangle 00_gl_triangle)
aga_add_gl_sample(01_gl_line 01_gl_line)
aga_add_gl_sample(02_gl_points 02_gl_points)

if(AGA_USE_VULKAN)
    find_package(Vulkan QUIET)
    find_program(AGA_GLSLC glslc)

    if(Vulkan_FOUND AND AGA_GLSLC)
        set(AGA_VK_SHADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
        file(MAKE_DIRECTORY "${AGA_VK_SHADER_DIR}")

        set(AGA_VK_VERT_SPV "${AGA_VK_SHADER_DIR}/primitive.vert.spv")
        set(AGA_VK_FRAG_SPV "${AGA_VK_SHADER_DIR}/primitive.frag.spv")

        add_custom_command(
            OUTPUT "${AGA_VK_VERT_SPV}"
            COMMAND "${AGA_GLSLC}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/primitive.vert"
                    -o "${AGA_VK_VERT_SPV}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/primitive.vert"
            VERBATIM
        )

        add_custom_command(
            OUTPUT "${AGA_VK_FRAG_SPV}"
            COMMAND "${AGA_GLSLC}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/primitive.frag"
                    -o "${AGA_VK_FRAG_SPV}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/primitive.frag"
            VERBATIM
        )

        add_custom_target(aga_vk_shaders
            DEPENDS "${AGA_VK_VERT_SPV}" "${AGA_VK_FRAG_SPV}"
        )

        function(aga_add_vk_sample name dir)
            add_executable(aga_sample_${name} ${dir}/main.cpp)

            target_link_libraries(aga_sample_${name}
                PRIVATE aga_core Vulkan::Vulkan glfw
            )

            target_compile_definitions(aga_sample_${name} PRIVATE
                AGA_VK_VERTEX_SHADER_PATH="${AGA_VK_VERT_SPV}"
                AGA_VK_FRAGMENT_SHADER_PATH="${AGA_VK_FRAG_SPV}"
            )

            target_include_directories(aga_sample_${name} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}"
            )

            add_dependencies(aga_sample_${name} aga_vk_shaders)
        endfunction()

        aga_add_vk_sample(10_vk_triangle 10_vk_triangle)
        aga_add_vk_sample(11_vk_line 11_vk_line)
        aga_add_vk_sample(12_vk_points 12_vk_points)
    else()
        message(WARNING
            "Vulkan samples disabled: Vulkan SDK and/or glslc was not found."
        )
    endif()
endif()