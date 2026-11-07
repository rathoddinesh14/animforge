#pragma once

#include <vishwakarma/graphics/IDevice.hpp>
#include <vishwakarma/graphics/Types.hpp>
#include <vishwakarma/platform/Window.hpp>

#include <memory>

namespace vishwakarma::graphics {

/**
 * Create an OpenGL device for an existing window. The device retains the
 * window's private lifetime state, so it may safely outlive the Window object.
 * Initialization failures are reported with exceptions.
 */
[[nodiscard]] std::unique_ptr<IDevice> createDevice(Backend backend, platform::Window& window);

} // namespace vishwakarma::graphics
