// OpenGL sample: colored triangle (NDC)
#include "common/run_primitive_sample.hpp"

int main() {
    return vishwakarma::samples::runOpenGLPrimitiveSample(
        "Vishwakarma - 00 Triangle", vishwakarma::samples::triangleGeometry(),
        &vishwakarma::graphics::IDevice::drawTriangles);
}
