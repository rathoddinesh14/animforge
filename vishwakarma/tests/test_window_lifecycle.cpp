#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/DeviceFactory.hpp>
#include <vishwakarma/platform/Window.hpp>

#include "common/Geometry.hpp"

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <thread>

TEST_CASE("Multiple windows share GLFW runtime ownership", "[window][lifecycle]") {
    auto first = std::make_unique<vishwakarma::platform::Window>(
        vishwakarma::platform::WindowDesc{640, 480, "first", false});
    vishwakarma::platform::Window second(
        vishwakarma::platform::WindowDesc{640, 480, "second", false});

    first.reset();
    second.pollEvents();
    REQUIRE_FALSE(second.shouldClose());
}

TEST_CASE("Framebuffer resize callback updates window dimensions", "[window][resize]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{640, 480, "resize", false});
    window.pollEvents();
    const int initialWidth = window.width();
    const int initialHeight = window.height();
    int callbackWidth = initialWidth;
    int callbackHeight = initialHeight;
    window.setResizeCallback(
        [&](int width, int height) {
            callbackWidth = width;
            callbackHeight = height;
        });

    window.setSize(800, 600);
    for (int attempt = 0; attempt < 100 &&
                          callbackWidth == initialWidth && callbackHeight == initialHeight;
         ++attempt) {
        window.pollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(callbackWidth != initialWidth);
    REQUIRE(callbackHeight != initialHeight);
    REQUIRE(callbackWidth == window.width());
    REQUIRE(callbackHeight == window.height());
    REQUIRE(callbackWidth > 0);
    REQUIRE(callbackHeight > 0);
}

TEST_CASE("Window rejects GLFW operations from another thread", "[window][threading]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{320, 240, "threading", false});
    bool rejected = false;
    std::thread otherThread([&] {
        try {
            window.pollEvents();
        } catch (const std::logic_error&) {
            rejected = true;
        }
    });
    otherThread.join();

    REQUIRE(rejected);
}

TEST_CASE("GLFW runtime rejects initialization from a second thread safely",
          "[window][threading]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{320, 240, "runtime thread", false});
    bool rejected = false;
    std::thread otherThread([&] {
        try {
            vishwakarma::platform::Window other(
                vishwakarma::platform::WindowDesc{320, 240, "invalid thread", false});
        } catch (const std::logic_error&) {
            rejected = true;
        }
    });
    otherThread.join();

    REQUIRE(rejected);
    REQUIRE_FALSE(window.shouldClose());
}

TEST_CASE("Resize callback exceptions are rethrown outside the GLFW callback",
          "[window][resize]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{320, 240, "resize exception", false});
    window.pollEvents();
    window.setResizeCallback([](int, int) { throw std::runtime_error("resize callback failure"); });
    window.setSize(500, 400);

    bool rethrown = false;
    for (int attempt = 0; attempt < 100 && !rethrown; ++attempt) {
        try {
            window.pollEvents();
        } catch (const std::runtime_error& error) {
            rethrown = std::string_view(error.what()) == "resize callback failure";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(rethrown);
}

TEST_CASE("OpenGL device survives its Window wrapper and shuts down cleanly",
          "[opengl][lifecycle]") {
    auto window = std::make_unique<vishwakarma::platform::Window>(
        vishwakarma::platform::WindowDesc{640, 480, "device", false});
    auto device = vishwakarma::graphics::createDevice(vishwakarma::graphics::Backend::OpenGL,
                                                      *window);

    REQUIRE(device != nullptr);
    REQUIRE(device->backend() == vishwakarma::graphics::Backend::OpenGL);

    window->pollEvents();
    const int initialWidth = window->width();
    const int initialHeight = window->height();
    bool resized = false;
    window->setResizeCallback([&](int width, int height) {
        device->resize(width, height);
        resized = width != initialWidth || height != initialHeight;
    });
    window->setSize(800, 600);
    for (int attempt = 0; attempt < 100 && !resized; ++attempt) {
        window->pollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    REQUIRE(resized);

    device->beginFrame(vishwakarma::graphics::Color{0.1f, 0.2f, 0.3f, 1.0f});
    const auto triangle = vishwakarma::samples::triangleGeometry();
    device->drawTriangles(triangle.data, triangle.vertexCount);
    device->endFrame();

    window.reset();
    device->beginFrame(vishwakarma::graphics::Color{0.0f, 0.0f, 0.0f, 1.0f});
    device->drawTriangles(triangle.data, triangle.vertexCount);
    device->endFrame();
    device.reset();
}

TEST_CASE("Window rejects invalid dimensions before initializing GLFW", "[window]") {
    REQUIRE_THROWS_AS(
        vishwakarma::platform::Window(
            vishwakarma::platform::WindowDesc{0, 480, "invalid", false}),
        std::invalid_argument);
}
