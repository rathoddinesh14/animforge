#pragma once

#include <aga/math/math.hpp>
#include <aga/rhi/Types.hpp>

#include <cstddef>
#include <string_view>

namespace aga::rhi {

/**
 * Minimal device contract for educational primitive samples.
 *
 * Samples pass CPU vertex data (pos + color). Backends upload and draw with the
 * requested topology. This is intentionally not a full engine RHI.
 *
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

    virtual void resize(int width, int height) = 0;

    /// Begin a frame; @p clearColor is linear RGBA in [0,1] (stored as xyzw).
    virtual void beginFrame(const math::Vec4& clearColor) = 0;

    /// Vertices use the interleaved layout: x y z r g b a.
    virtual void drawPoints(const float* vertices, std::size_t vertexCount) = 0;

    /// Vertices are interpreted as independent pairs.
    virtual void drawLines(const float* vertices, std::size_t vertexCount) = 0;

    /// Vertices are interpreted as independent triangles.
    virtual void drawTriangles(const float* vertices, std::size_t vertexCount) = 0;

    virtual void endFrame() = 0;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

protected:
    IDevice() = default;
};

} // namespace aga::rhi
