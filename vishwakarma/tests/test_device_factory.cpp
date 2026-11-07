#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/DeviceFactory.hpp>

TEST_CASE("Vulkan device creation returns null when backend is unimplemented", "[rhi][factory]") {
    const vishwakarma::graphics::DeviceDesc desc{};

    REQUIRE(vishwakarma::graphics::createDevice(vishwakarma::graphics::Backend::Vulkan, nullptr,
                                               desc) == nullptr);
}

#ifdef VISHWAKARMA_USE_OPENGL
TEST_CASE("OpenGL device creation rejects a null window", "[rhi][factory][opengl]") {
    const vishwakarma::graphics::DeviceDesc desc{};

    REQUIRE_THROWS_AS(vishwakarma::graphics::createDevice(
                          vishwakarma::graphics::Backend::OpenGL, nullptr, desc),
                      std::runtime_error);
}
#endif
