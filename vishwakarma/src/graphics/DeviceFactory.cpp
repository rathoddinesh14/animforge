#include <vishwakarma/graphics/DeviceFactory.hpp>

#include "../backends/opengl/OpenGLDevice.hpp"

namespace vishwakarma::graphics {

std::unique_ptr<IDevice> createDevice(Backend backend, GLFWwindow* window, const DeviceDesc& desc) {
    switch (backend) {
    case Backend::OpenGL:
        return std::make_unique<OpenGLDevice>(window, desc);
    case Backend::Vulkan:
        // Not implemented yet — Phase after OpenGL triangle + DebugDraw.
        return nullptr;
    }
    return nullptr;
}

} // namespace vishwakarma::graphics
