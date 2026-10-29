#include <catch2/catch_test_macros.hpp>

#include <aga/rhi/DeviceFactory.hpp>

TEST_CASE("Vulkan device creation returns null when backend is unimplemented", "[rhi][factory]") {
    const aga::rhi::DeviceDesc desc{};

    REQUIRE(aga::rhi::createDevice(aga::rhi::Backend::Vulkan, nullptr, desc) == nullptr);
}

#ifdef AGA_USE_OPENGL
TEST_CASE("OpenGL device creation rejects a null window", "[rhi][factory][opengl]") {
    const aga::rhi::DeviceDesc desc{};

    REQUIRE_THROWS_AS(aga::rhi::createDevice(aga::rhi::Backend::OpenGL, nullptr, desc),
                      std::runtime_error);
}
#endif
