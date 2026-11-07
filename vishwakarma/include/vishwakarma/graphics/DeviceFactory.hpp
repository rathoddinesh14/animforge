#pragma once

#include <vishwakarma/graphics/IDevice.hpp>
#include <vishwakarma/graphics/Types.hpp>

#include <memory>

struct GLFWwindow; // opaque; platform owns the window

namespace vishwakarma::graphics {

/**
 * Create a graphics device for the given backend, bound to an existing GLFW window.
 * @return nullptr if the requested backend is not implemented. Initialization
 *         failures from an implemented backend are reported with exceptions.
 */
[[nodiscard]] std::unique_ptr<IDevice> createDevice(Backend backend, GLFWwindow* window,
                                                    const DeviceDesc& desc);

} // namespace vishwakarma::graphics
