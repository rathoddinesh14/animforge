#include <aga/rhi/DeviceFactory.hpp>

#include "opengl/OpenGLDevice.hpp"

namespace aga::rhi {

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

} // namespace aga::rhi
