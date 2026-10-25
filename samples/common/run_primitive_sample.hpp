#pragma once

/**
 * Shared main-loop for OpenGL primitive samples.
 * Each sample only chooses title, topology, and geometry.
 */

#include <aga/platform/Window.hpp>
#include <aga/rhi/DeviceFactory.hpp>
#include <aga/rhi/Types.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

#include "common/Geometry.hpp"

namespace aga::samples {

inline int runOpenGLPrimitiveSample(std::string_view windowTitle,
                                    aga::rhi::PrimitiveTopology topology,
                                    GeometryView geometry) {
    try {
        aga::platform::WindowDesc wdesc;
        wdesc.title = std::string(windowTitle);
        wdesc.width = 1280;
        wdesc.height = 720;
        aga::platform::Window window(wdesc);

        aga::rhi::DeviceDesc ddesc;
        ddesc.framebufferWidth = window.width();
        ddesc.framebufferHeight = window.height();

        auto device = aga::rhi::createDevice(aga::rhi::Backend::OpenGL, window.handle(), ddesc);
        if (!device) {
            std::fprintf(stderr, "Failed to create OpenGL device\n");
            return EXIT_FAILURE;
        }

        window.setResizeCallback([&](int w, int h) { device->resize(w, h); });
        std::printf("Backend: %.*s\n", static_cast<int>(device->name().size()),
                    device->name().data());

        while (!window.shouldClose()) {
            window.pollEvents();
            device->beginFrame(aga::math::Vec4{0.08f, 0.08f, 0.1f, 1.0f});
            device->draw(topology, geometry.data, geometry.vertexCount);
            device->endFrame();
            window.swapBuffers();
        }
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Fatal: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

} // namespace aga::samples
