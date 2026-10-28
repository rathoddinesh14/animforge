# AnimForge unit tests (Catch2 v3)

add_executable(aga_tests
    math/test_glm_integration.cpp
    math/test_math_helpers.cpp
    rhi/test_rhi_types.cpp
    rhi/test_draw_validation.cpp
    samples/test_geometry.cpp
)

target_link_libraries(aga_tests
    PRIVATE
        aga_core
        Catch2::Catch2WithMain
        glm::glm
)

target_include_directories(aga_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/samples
)

include(Catch)
catch_discover_tests(aga_tests)
