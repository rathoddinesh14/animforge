/**
 * Sample 00: Hello Triangle
 *
 * Draws a single colored triangle through aga::rhi::IDevice (OpenGL backend).
 * Same call pattern will target Vulkan once that backend is implemented.
 *
 * Run:  aga_sample_00_hello_triangle
 * Esc / close window to exit.
 */

#include <aga/platform/Window.hpp>
#include <aga/rhi/DeviceFactory.hpp>

#include <cstdio>
#include <cstdlib>

namespace {

// Interleaved pos.xyz + color.rgba (NDC triangle)
constexpr float kTriangle[] = {
    // x, y, z, r, g, b, a
    0.0f,  0.5f, 0.0f, 1.0f, 0.2f, 0.2f, 1.0f, // top - red
    -0.5f, -0.5f, 0.0f, 0.2f, 1.0f, 0.2f, 1.0f, // bottom-left - green
    0.5f,  -0.5f, 0.0f, 0.2f, 0.2f, 1.0f, 1.0f, // bottom-right - blue
};

} // namespace

int main() {
    try {
        aga::platform::WindowDesc wdesc;
        wdesc.title = "AnimForge - 00 Hello Triangle (OpenGL)";
        wdesc.width = 1280;
        wdesc.height = 720;
        aga::platform::Window window(wdesc);

        aga::rhi::DeviceDesc ddesc;
        ddesc.framebufferWidth = window.width();
        ddesc.framebufferHeight = window.height();
        ddesc.debug = true;

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
            device->drawTriangleList(kTriangle, 3);
            device->endFrame();

            window.swapBuffers();
        }
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Fatal: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
