// OpenGL sample: single point (NDC)
#include "common/run_primitive_sample.hpp"

int main(int argc, char** argv) {
    return aga::samples::runOpenGLPrimitiveSample(
        "AnimForge - 02 GL Points", aga::samples::pointGeometry(),
        &vishwakarma::graphics::IDevice::drawPoints, argc, argv);
}
