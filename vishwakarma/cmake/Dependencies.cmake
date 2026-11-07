include(FetchContent)

function(vishwakarma_resolve_glfw)
    find_package(glfw3 3.3 CONFIG QUIET)

    if(TARGET glfw)
        set(VISHWAKARMA_GLFW_TARGET glfw PARENT_SCOPE)
        return()
    elseif(TARGET glfw3::glfw)
        set(VISHWAKARMA_GLFW_TARGET glfw3::glfw PARENT_SCOPE)
        return()
    endif()

    if(UNIX AND NOT APPLE)
        find_package(X11 REQUIRED COMPONENTS Xrandr Xinerama Xi Xcursor)
    endif()

    message(STATUS "GLFW not found - fetching GLFW 3.4")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    if(TARGET glfw)
        set(VISHWAKARMA_GLFW_TARGET glfw PARENT_SCOPE)
    else()
        message(FATAL_ERROR "GLFW was fetched but did not define the glfw target")
    endif()
endfunction()

if(VISHWAKARMA_BUILD_TESTS)
    find_package(Catch2 3 CONFIG QUIET)
    if(NOT Catch2_FOUND)
        message(STATUS "Catch2 not found - fetching Catch2 3.7.1")
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.7.1
            GIT_SHALLOW TRUE
        )
        set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
        set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
        set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(Catch2)
        list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    endif()
endif()
