// Dummy translation unit so that aga_core is not an empty static library.
// Real implementation files will replace this later.
#include <aga/math/math.hpp>

namespace aga::math {
    // Force the compiler to emit something
    void detail_force_link() {}
}