# AnimForge unit tests (Catch2 v3)

add_executable(aga_tests
    math/test_glm_integration.cpp
    math/test_math_helpers.cpp
    samples/test_geometry.cpp
)

if(AGA_USE_OPENGL)
    target_sources(aga_tests PRIVATE graphics/test_vishwakarma_integration.cpp)
endif()

target_link_libraries(aga_tests
    PRIVATE
        aga_core
        Catch2::Catch2WithMain
        glm::glm
)

if(AGA_USE_OPENGL)
    target_link_libraries(aga_tests PRIVATE Vishwakarma::Graphics)
endif()

target_include_directories(aga_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/samples
)

include(Catch)
catch_discover_tests(aga_tests)
