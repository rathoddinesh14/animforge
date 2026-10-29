// OpenGL sample: single point (NDC)
#include "common/run_primitive_sample.hpp"

int main() {
    return aga::samples::runOpenGLPrimitiveSample("AnimForge - 02 GL Points",
                                                  aga::rhi::PrimitiveTopology::PointList,
                                                  aga::samples::pointGeometry());
}
