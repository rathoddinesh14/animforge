#pragma once

#include <cstddef>

namespace vishwakarma::samples {

/**
 * Shared educational geometry for primitive samples.
 * Layout: interleaved x y z r g b a (7 floats per vertex), NDC coordinates.
 *
 * The primitive samples share this layout across their draw modes.
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

} // namespace vishwakarma::samples
