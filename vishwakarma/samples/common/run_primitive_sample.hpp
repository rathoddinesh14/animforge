
#pragma once

#include <vishwakarma/platform/Window.hpp>
#include <vishwakarma/graphics/DeviceFactory.hpp>
#include <vishwakarma/graphics/Types.hpp>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string>
#include <string_view>

#include "common/Geometry.hpp"

namespace vishwakarma::samples {

using PrimitiveDrawFunction = void (vishwakarma::graphics::IDevice::*)(const float*, std::size_t);

inline int runOpenGLPrimitiveSample(std::string_view windowTitle, GeometryView geometry,
                                    PrimitiveDrawFunction drawFunction) {

    try {
        vishwakarma::platform::WindowDesc wdesc;
        wdesc.title = std::string(windowTitle);
        wdesc.width = 1280;
        wdesc.height = 720;

        vishwakarma::platform::Window window(wdesc);

        vishwakarma::graphics::DeviceDesc ddesc;
        ddesc.framebufferWidth = window.width();
        ddesc.framebufferHeight = window.height();

        auto device = vishwakarma::graphics::createDevice(
            vishwakarma::graphics::Backend::OpenGL, window.handle(), ddesc);

        if (!device) {
            std::fprintf(stderr, "Failed to create OpenGL device\n");
            return EXIT_FAILURE;
        }

        window.setResizeCallback([&](int width, int height) { device->resize(width, height); });

        std::printf("Backend: %.*s\n", static_cast<int>(device->name().size()),
                    device->name().data());

        while (!window.shouldClose()) {
            window.pollEvents();

            device->beginFrame(vishwakarma::graphics::Color{0.08f, 0.08f, 0.1f, 1.0f});

            (device.get()->*drawFunction)(geometry.data, geometry.vertexCount);

            device->endFrame();
            window.swapBuffers();
        }
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Fatal: %s\n", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

} // namespace vishwakarma::samples
