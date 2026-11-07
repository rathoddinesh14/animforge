include(Catch)

add_executable(vishwakarma_tests
    test_device_factory.cpp
    test_draw_validation.cpp
    test_rhi_types.cpp
    test_geometry.cpp
)
target_include_directories(vishwakarma_tests PRIVATE "${CMAKE_SOURCE_DIR}/samples")
target_link_libraries(vishwakarma_tests PRIVATE
    Vishwakarma::Graphics
    Catch2::Catch2WithMain
)
vishwakarma_set_warnings(vishwakarma_tests)

catch_discover_tests(vishwakarma_tests)
