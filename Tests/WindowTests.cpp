#include <doctest/doctest.h>

#include "Window.h"

TEST_CASE("window aspect ratio is safe when framebuffer height is zero")
{
	ve::engine::WindowCreateInfo create_info{};
	create_info.width = 640;
	create_info.height = 0;

	const ve::engine::Window window{ create_info };

	CHECK(window.GetHeight() == 1);
	CHECK(window.GetAspectRatio() == doctest::Approx(640.0f));
}
