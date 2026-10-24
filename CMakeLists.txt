cmake_minimum_required(VERSION 3.25)
project(AnimForge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(AGA_BUILD_SAMPLES "Build sample applications" ON)
option(AGA_BUILD_TESTS "Build unit tests" ON)
option(AGA_USE_OPENGL "Enable OpenGL backend" ON)
option(AGA_USE_VULKAN "Enable Vulkan backend" OFF)

# Warnings
include(cmake/CompilerWarnings.cmake)

# Dependencies (vcpkg or FetchContent)
include(cmake/Dependencies.cmake)

# Core library
add_library(aga_core STATIC)
target_include_directories(aga_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(aga_core
    PUBLIC
        glm::glm
)

target_sources(aga_core PRIVATE
    src/math/math_dummy.cpp
)

# OpenGL RHI + platform (window)
if(AGA_USE_OPENGL)
    target_sources(aga_core PRIVATE
        src/platform/Window.cpp
        src/rhi/DeviceFactory.cpp
        src/rhi/opengl/OpenGLDevice.cpp
    )
    target_link_libraries(aga_core
        PUBLIC
            glfw
            glad_gl_core_45
    )
    target_compile_definitions(aga_core PUBLIC AGA_USE_OPENGL=1)
    target_include_directories(aga_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

# Samples
if(AGA_BUILD_SAMPLES)
    if(NOT AGA_USE_OPENGL)
        message(WARNING "AGA_BUILD_SAMPLES is ON but AGA_USE_OPENGL is OFF - samples may not link")
    endif()
    add_subdirectory(samples)
endif()

# Tests
if(AGA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "AnimForge configured")
message(STATUS "  OpenGL : ${AGA_USE_OPENGL}")
message(STATUS "  Vulkan : ${AGA_USE_VULKAN}")
message(STATUS "  Samples: ${AGA_BUILD_SAMPLES}")
message(STATUS "  Tests  : ${AGA_BUILD_TESTS}")
