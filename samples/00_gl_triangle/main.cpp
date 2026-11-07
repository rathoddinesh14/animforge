// OpenGL sample: colored triangle (NDC)
#include "common/run_primitive_sample.hpp"

int main(int argc, char** argv) {
    return aga::samples::runOpenGLPrimitiveSample(
        "AnimForge - 00 GL Triangle", aga::samples::triangleGeometry(),
        &vishwakarma::graphics::IDevice::drawTriangles, argc, argv);
}
