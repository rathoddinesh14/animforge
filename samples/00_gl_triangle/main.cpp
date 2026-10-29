// OpenGL sample: colored triangle (NDC)
#include "common/run_primitive_sample.hpp"

int main() {
    return aga::samples::runOpenGLPrimitiveSample("AnimForge - 00 GL Triangle",
                                                  aga::samples::triangleGeometry(),
                                                &aga::rhi::IDevice::drawTriangles);
}
