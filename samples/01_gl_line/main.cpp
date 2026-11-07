// OpenGL sample: single line segment (NDC)
#include "common/run_primitive_sample.hpp"

int main(int argc, char** argv) {
    return aga::samples::runOpenGLPrimitiveSample(
        "AnimForge - 01 GL Line", aga::samples::lineGeometry(),
        &vishwakarma::graphics::IDevice::drawLines, argc, argv);
}
