cmake_minimum_required(VERSION 3.25)
project(Vishwakarma VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VISHWAKARMA_BUILD_SAMPLES "Build the OpenGL triangle sample" ON)
option(VISHWAKARMA_BUILD_TESTS "Build unit tests" ON)

include(cmake/Dependencies.cmake)
include(cmake/CompilerWarnings.cmake)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
vishwakarma_resolve_glfw()

add_library(vishwakarma_graphics STATIC
    src/platform/Window.cpp
    src/graphics/DeviceFactory.cpp
    src/backends/opengl/OpenGLDevice.cpp
    src/backends/opengl/GlLoader.cpp
)
add_library(Vishwakarma::Graphics ALIAS vishwakarma_graphics)

target_include_directories(vishwakarma_graphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(vishwakarma_graphics
    PRIVATE
        ${VISHWAKARMA_GLFW_TARGET}
        OpenGL::GL
        Threads::Threads
)
target_compile_definitions(vishwakarma_graphics PRIVATE GLFW_INCLUDE_NONE=1)
vishwakarma_set_warnings(vishwakarma_graphics)

if(VISHWAKARMA_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(VISHWAKARMA_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

message(STATUS "Vishwakarma configured")
message(STATUS "  Samples: ${VISHWAKARMA_BUILD_SAMPLES}")
message(STATUS "  Tests  : ${VISHWAKARMA_BUILD_TESTS}")
