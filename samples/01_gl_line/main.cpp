// OpenGL sample: single line segment (NDC)
#include "common/run_primitive_sample.hpp"

int main() {
    return aga::samples::runOpenGLPrimitiveSample(
        "AnimForge - 01 GL Line", aga::samples::lineGeometry(), &aga::rhi::IDevice::drawLines);
}
