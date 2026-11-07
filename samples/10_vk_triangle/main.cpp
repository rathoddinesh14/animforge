#include "common/Geometry.hpp"
#include "common/run_vulkan_primitive_sample.hpp"

int main()
{
    return aga::samples::runVulkanTriangleSample(
        aga::samples::triangleGeometry());
}