#include <vishwakarma/graphics/DeviceFactory.hpp>

#include "../backends/opengl/OpenGLDevice.hpp"
#include "../platform/WindowState.hpp"

#include <stdexcept>

namespace vishwakarma::graphics {

std::unique_ptr<IDevice> createDevice(Backend backend, platform::Window& window) {
    switch (backend) {
    case Backend::OpenGL:
        return std::make_unique<OpenGLDevice>(detail::WindowAccess::retain(window));
    }
    throw std::invalid_argument("Unsupported graphics backend");
}

} // namespace vishwakarma::graphics
