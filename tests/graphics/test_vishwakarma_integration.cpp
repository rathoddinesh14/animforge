#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/DeviceFactory.hpp>
#include <vishwakarma/platform/Window.hpp>

#include <string_view>

TEST_CASE("AnimForge OpenGL sample API creates, resizes, renders, and shuts down",
          "[graphics][integration]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{320, 240, "AnimForge integration", false});
    auto device =
        vishwakarma::graphics::createDevice(vishwakarma::graphics::Backend::OpenGL, window);

    REQUIRE(device->backend() == vishwakarma::graphics::Backend::OpenGL);
    REQUIRE(device->name() == "OpenGL");

    device->resize(640, 480);
    device->beginFrame(vishwakarma::graphics::Color{0.08f, 0.08f, 0.1f, 1.0f});
    constexpr float triangle[] = {
        0.0f, 0.5f, 0.0f, 1.0f, 0.2f,  0.2f, 1.0f, -0.5f, -0.5f, 0.0f, 0.2f,
        1.0f, 0.2f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f, 0.2f,  1.0f,  1.0f,
    };
    device->drawTriangles(triangle, 3);
    device->endFrame();

    device.reset();
    window.pollEvents();
}
