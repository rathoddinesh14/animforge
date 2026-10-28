# Resolve dependencies: prefer packages already on the system / vcpkg toolchain,
# otherwise fetch via FetchContent (used by CI on the free tier).

include(FetchContent)

# ---------------------------------------------------------------------------
# GLM
# ---------------------------------------------------------------------------
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
    message(STATUS "glm not found via find_package - fetching with FetchContent")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    TRUE
    )
    set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glm)
    if(NOT TARGET glm::glm)
        add_library(glm::glm ALIAS glm)
    endif()
endif()

# ---------------------------------------------------------------------------
# Catch2 (only when tests are enabled)
# ---------------------------------------------------------------------------
if(AGA_BUILD_TESTS)
    find_package(Catch2 3 CONFIG QUIET)
    if(NOT Catch2_FOUND)
        message(STATUS "Catch2 not found via find_package - fetching with FetchContent")
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.7.1
            GIT_SHALLOW    TRUE
        )
        set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
        set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(Catch2)
        list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
    endif()
endif()

# ---------------------------------------------------------------------------
# GLFW + GLAD (OpenGL samples / RHI)
# ---------------------------------------------------------------------------
if(AGA_USE_OPENGL OR AGA_BUILD_SAMPLES)
    if(UNIX AND NOT APPLE)
        find_package(X11 REQUIRED COMPONENTS Xrandr Xinerama Xi Xcursor)
    endif()

    find_package(glfw3 CONFIG QUIET)
    if(NOT glfw3_FOUND AND NOT TARGET glfw)
        message(STATUS "glfw not found via find_package - fetching with FetchContent")
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.4
            GIT_SHALLOW    TRUE
        )
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glfw)
    endif()

endif()
