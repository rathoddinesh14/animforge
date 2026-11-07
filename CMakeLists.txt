cmake_minimum_required(VERSION 3.25)
project(AnimForge VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(AGA_BUILD_SAMPLES "Build sample applications" ON)
option(AGA_BUILD_TESTS "Build unit tests" ON)
option(AGA_USE_OPENGL "Build OpenGL samples and tests through Vishwakarma" ON)
option(AGA_USE_VULKAN "Build Vulkan-specific samples" ON)
option(AGA_ENABLE_COVERAGE "Enable code coverage instrumentation for tests" OFF)

if(AGA_BUILD_TESTS)
    include(CTest)
endif()

# Warnings
include(cmake/CompilerWarnings.cmake)

if(AGA_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)
    else()
        message(WARNING "Code coverage is only supported with GCC/Clang toolchains.")
    endif()
endif()

# Dependencies (system packages or FetchContent)
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

# Vulkan samples use GLFW directly. OpenGL infrastructure is provided by
# Vishwakarma and is fetched only when the OpenGL consumers are enabled.
if(AGA_USE_OPENGL AND (AGA_BUILD_SAMPLES OR AGA_BUILD_TESTS))
    include(FetchContent)
    set(VISHWAKARMA_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(VISHWAKARMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        vishwakarma
        GIT_REPOSITORY https://github.com/rathoddinesh14/vishwakarma.git
        GIT_TAG        2b5f2f8a156f669552a60973ea9ab1a15c47ff97
        GIT_SHALLOW    FALSE
    )
    FetchContent_MakeAvailable(vishwakarma)
endif()

# Samples
if(AGA_BUILD_SAMPLES)
    if(NOT AGA_USE_OPENGL AND NOT AGA_USE_VULKAN)
        message(WARNING "AGA_BUILD_SAMPLES is ON but both graphics sample options are OFF")
    endif()
    add_subdirectory(samples)
endif()

# Tests
if(AGA_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(AGA_ENABLE_COVERAGE)
    find_program(LCOV_EXECUTABLE NAMES lcov REQUIRED)
    set(AGA_COVERAGE_BASE_FILE ${CMAKE_BINARY_DIR}/coverage-base.info)
    set(AGA_COVERAGE_RUN_FILE ${CMAKE_BINARY_DIR}/coverage-run.info)
    set(AGA_COVERAGE_MERGED_FILE ${CMAKE_BINARY_DIR}/coverage-merged.info)
    set(AGA_COVERAGE_INFO_FILE ${CMAKE_SOURCE_DIR}/.coverage/coverage.info)
    add_custom_target(aga_coverage_report
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/.coverage
        COMMAND ${LCOV_EXECUTABLE} --capture
            --demangle-cpp
            --initial
            --directory ${CMAKE_BINARY_DIR}/CMakeFiles/aga_core.dir
            --output-file ${AGA_COVERAGE_BASE_FILE}
        COMMAND ${LCOV_EXECUTABLE} --capture
            --demangle-cpp
            --ignore-errors source
            --directory ${CMAKE_BINARY_DIR}/CMakeFiles/aga_core.dir
            --directory ${CMAKE_BINARY_DIR}/tests/CMakeFiles/aga_tests.dir
            --output-file ${AGA_COVERAGE_RUN_FILE}
        COMMAND ${LCOV_EXECUTABLE} --add-tracefile ${AGA_COVERAGE_BASE_FILE}
            --add-tracefile ${AGA_COVERAGE_RUN_FILE}
            --output-file ${AGA_COVERAGE_MERGED_FILE}
        COMMAND ${LCOV_EXECUTABLE} --remove
            ${AGA_COVERAGE_MERGED_FILE}
            "${CMAKE_BINARY_DIR}/*"
            "${CMAKE_SOURCE_DIR}/tests/*"
            "/usr/*"
            --output-file ${AGA_COVERAGE_INFO_FILE}
        DEPENDS aga_tests
        VERBATIM
    )
endif()

message(STATUS "AnimForge configured")
message(STATUS "  OpenGL : ${AGA_USE_OPENGL}")
message(STATUS "  Vulkan : ${AGA_USE_VULKAN}")
message(STATUS "  Samples: ${AGA_BUILD_SAMPLES}")
message(STATUS "  Tests  : ${AGA_BUILD_TESTS}")
