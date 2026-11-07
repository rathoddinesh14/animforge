#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/DeviceFactory.hpp>
#include <vishwakarma/platform/Window.hpp>

#include <stdexcept>

TEST_CASE("Factory rejects an unknown backend explicitly", "[graphics][factory]") {
    vishwakarma::platform::Window window(
        vishwakarma::platform::WindowDesc{320, 240, "factory", false});
    const auto invalidBackend = static_cast<vishwakarma::graphics::Backend>(255);

    REQUIRE_THROWS_AS(vishwakarma::graphics::createDevice(invalidBackend, window),
                      std::invalid_argument);
}
