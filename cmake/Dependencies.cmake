# Dependencies resolved via vcpkg toolchain.
# This file centralises find_package calls so the root CMakeLists stays clean.

find_package(glm CONFIG REQUIRED)
find_package(Catch2 3 CONFIG REQUIRED)

# Optional later
# find_package(glfw3 CONFIG REQUIRED)
# find_package(glad CONFIG REQUIRED)
# find_package(Vulkan REQUIRED)
