
#pragma once

#include <vishwakarma/graphics/DeviceFactory.hpp>
#include <vishwakarma/graphics/Types.hpp>
#include <vishwakarma/platform/Window.hpp>

#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <string_view>

#include "common/Geometry.hpp"

namespace aga::samples {

using PrimitiveDrawFunction = void (vishwakarma::graphics::IDevice::*)(const float*, std::size_t);

inline int runOpenGLPrimitiveSample(std::string_view windowTitle, GeometryView geometry,
                                    PrimitiveDrawFunction drawFunction, int argc, char** argv) {
    std::size_t maxFrames = 0;
    if (argc == 3 && std::string_view(argv[1]) == "--frames") {
        const auto* end = argv[2] + std::char_traits<char>::length(argv[2]);
        const auto [parsedEnd, error] = std::from_chars(argv[2], end, maxFrames);
        if (error != std::errc{} || parsedEnd != end || maxFrames == 0) {
            std::fprintf(stderr, "Usage: sample [--frames positive-count]\n");
            return EXIT_FAILURE;
        }
    } else if (argc != 1) {
        std::fprintf(stderr, "Usage: sample [--frames positive-count]\n");
        return EXIT_FAILURE;
    }

    try {
        vishwakarma::platform::WindowDesc wdesc;
        wdesc.title = std::string(windowTitle);
        wdesc.width = 1280;
        wdesc.height = 720;

        vishwakarma::platform::Window window(wdesc);
        auto device =
            vishwakarma::graphics::createDevice(vishwakarma::graphics::Backend::OpenGL, window);

        window.setResizeCallback([&](int width, int height) { device->resize(width, height); });

        std::printf("Backend: %.*s\n", static_cast<int>(device->name().size()),
                    device->name().data());

        std::size_t renderedFrames = 0;
        while (!window.shouldClose() && (maxFrames == 0 || renderedFrames < maxFrames)) {
            window.pollEvents();

            device->beginFrame(vishwakarma::graphics::Color{0.08f, 0.08f, 0.1f, 1.0f});

            (device.get()->*drawFunction)(geometry.data, geometry.vertexCount);

            device->endFrame();
            window.swapBuffers();
            ++renderedFrames;
        }
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Fatal: %s\n", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

} // namespace aga::samples
