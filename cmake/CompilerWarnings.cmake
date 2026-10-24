# Compiler warnings - treat as errors in CI later
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    # /WX can be enabled once the codebase is clean
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()