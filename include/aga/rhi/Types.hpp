#pragma once

#include <cstdint>

namespace aga::rhi {

/// Graphics API selected at runtime via the device factory.
enum class Backend : std::uint8_t { OpenGL = 0, Vulkan = 1 };

/// Descriptor for device creation (window already exists; device attaches to it).
struct DeviceDesc {
    int framebufferWidth = 1280;
    int framebufferHeight = 720;
    bool debug = true; // GL debug context / Vulkan validation when available
};

} // namespace aga::rhi
