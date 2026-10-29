#pragma once

#include <aga/rhi/Types.hpp>

#include <cstddef>

namespace aga::rhi {

/**
 * Validate vertex counts for educational draw calls (no GPU required).
 * Used by backends and unit tests.
 */
[[nodiscard]] inline bool isValidVertexCount(PrimitiveTopology topology,
                                             std::size_t vertexCount) noexcept {
    if (vertexCount == 0) {
        return false;
    }
    switch (topology) {
    case PrimitiveTopology::PointList:
        return true;
    case PrimitiveTopology::LineList:
        return (vertexCount % 2) == 0;
    case PrimitiveTopology::LineStrip:
        return vertexCount >= 2;
    case PrimitiveTopology::TriangleList:
        return (vertexCount % 3) == 0;
    case PrimitiveTopology::TriangleStrip:
        return vertexCount >= 3;
    }
    return false;
}

/// Floats per interleaved sample vertex: x y z r g b a
inline constexpr std::size_t kVertexFloats = 7;

} // namespace aga::rhi
