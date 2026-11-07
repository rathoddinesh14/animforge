// OpenGL sample: colored triangle (NDC)
#include "common/run_primitive_sample.hpp"

#include <charconv>
#include <cstdio>
#include <string_view>

int main(int argc, char** argv) {
    std::size_t maxFrames = 0;
    if (argc == 3 && std::string_view(argv[1]) == "--frames") {
        const auto* end = argv[2] + std::char_traits<char>::length(argv[2]);
        const auto [parsedEnd, error] = std::from_chars(argv[2], end, maxFrames);
        if (error != std::errc{} || parsedEnd != end || maxFrames == 0) {
            std::fprintf(stderr, "Usage: vishwakarma_triangle [--frames positive-count]\n");
            return 2;
        }
    } else if (argc != 1) {
        std::fprintf(stderr, "Usage: vishwakarma_triangle [--frames positive-count]\n");
        return 2;
    }

    return vishwakarma::samples::runOpenGLPrimitiveSample(
        "Vishwakarma - 00 Triangle", vishwakarma::samples::triangleGeometry(),
        &vishwakarma::graphics::IDevice::drawTriangles, maxFrames);
}
