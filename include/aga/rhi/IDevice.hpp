#pragma once

#include <aga/math/math.hpp>
#include <aga/rhi/Types.hpp>

#include <memory>
#include <string_view>

namespace aga::rhi {

/**
 * Minimal graphics device contract for debug primitives and early samples.
 *
 * Intentionally small: clear frame, draw a triangle list from CPU vertices,
 * present. Full buffer/pipeline objects come when DebugDraw and mesh paths need them.
 *
 * Implementations: OpenGLDevice (first), VulkanDevice (later).
 * Public headers must not include GL or Vulkan headers.
 */
class IDevice {
public:
    virtual ~IDevice() = default;

    IDevice(const IDevice&) = delete;
    IDevice& operator=(const IDevice&) = delete;
    IDevice(IDevice&&) = delete;
    IDevice& operator=(IDevice&&) = delete;

    [[nodiscard]] virtual Backend backend() const noexcept = 0;

    /// Resize internal viewport / default framebuffer tracking.
    virtual void resize(int width, int height) = 0;

    /// Begin a frame; @p clearColor is linear RGBA in [0,1].
    virtual void beginFrame(const math::Vec4& clearColor) = 0;

    /**
     * Draw a triangle list from interleaved pos(float3) + color(float4) vertices.
     * @param vertices  Pointer to tightly packed floats: x y z r g b a per vertex.
     * @param vertexCount  Number of vertices (must be multiple of 3 for triangle list).
     *
     * Educational path: uploads every call. Production DebugDraw will batch later.
     */
    virtual void drawTriangleList(const float* vertices, std::size_t vertexCount) = 0;

    /// Finish GPU work for the frame and present (swapchain / glfw swap).
    virtual void endFrame() = 0;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

protected:
    IDevice() = default;
};

} // namespace aga::rhi
