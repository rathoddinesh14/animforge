#pragma once

#include <aga/rhi/IDevice.hpp>
#include <aga/rhi/Types.hpp>

#include <memory>

struct GLFWwindow; // opaque; platform owns the window

namespace aga::rhi {

/**
 * Create a graphics device for the given backend, bound to an existing GLFW window.
 * @return nullptr if the backend is not compiled in or initialization fails.
 */
[[nodiscard]] std::unique_ptr<IDevice> createDevice(Backend backend, GLFWwindow* window,
                                                    const DeviceDesc& desc);

} // namespace aga::rhi
