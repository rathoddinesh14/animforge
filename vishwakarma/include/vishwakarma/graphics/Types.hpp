#pragma once

#include <cstdint>
#include <array>

namespace vishwakarma::graphics {

/// Graphics API selected at runtime via the device factory.
enum class Backend : std::uint8_t { OpenGL = 0, Vulkan = 1 };

/// Linear RGBA clear color with components in [0, 1].
using Color = std::array<float, 4>;

/// Descriptor for device creation (window already exists; device attaches to it).
struct DeviceDesc {
    int framebufferWidth = 1280;
    int framebufferHeight = 720;
    bool debug = true; // GL debug context / Vulkan validation when available
};

} // namespace vishwakarma::graphics
