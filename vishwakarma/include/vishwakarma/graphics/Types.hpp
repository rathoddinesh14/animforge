#pragma once

#include <array>
#include <cstdint>

namespace vishwakarma::graphics {

enum class Backend : std::uint8_t { OpenGL = 0 };

/// Linear RGBA clear color with components in [0, 1].
using Color = std::array<float, 4>;

} // namespace vishwakarma::graphics
