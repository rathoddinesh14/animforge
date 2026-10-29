#pragma once

#include <cstddef>

namespace aga::samples {

/**
 * Shared educational geometry for primitive samples.
 * Layout: interleaved x y z r g b a (7 floats per vertex), NDC coordinates.
 *
 * Same data is used by OpenGL and Vulkan samples so the only variable is the API.
 */
struct GeometryView {
    const float* data = nullptr;
    std::size_t vertexCount = 0;
};

// Single point at origin (white): x y z r g b a
inline constexpr float kPointVertices[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

// Line left -> right (cyan): two vertices
inline constexpr float kLineVertices[] = {
    -0.75f, 0.0f, 0.0f, 0.2f, 0.9f, 1.0f, 1.0f, 0.75f, 0.0f, 0.0f, 0.2f, 0.9f, 1.0f, 1.0f,
};

// Triangle: top red, bottom-left green, bottom-right blue
inline constexpr float kTriangleVertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f, 0.2f,  0.2f, 1.0f, -0.5f, -0.5f, 0.0f, 0.2f,
    1.0f, 0.2f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f, 0.2f,  1.0f,  1.0f,
};

inline GeometryView pointGeometry() {
    return {kPointVertices, 1};
}

inline GeometryView lineGeometry() {
    return {kLineVertices, 2};
}

inline GeometryView triangleGeometry() {
    return {kTriangleVertices, 3};
}

} // namespace aga::samples
