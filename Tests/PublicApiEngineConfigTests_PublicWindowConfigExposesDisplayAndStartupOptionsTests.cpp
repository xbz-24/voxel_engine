#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public window config exposes display and startup options")
{
	const voxel::WindowConfig config = voxel::Fullscreen("Display API")
		.UseMonitor(1)
		.WithRefreshRate(144)
		.SetResizable(false)
		.EnableHighDpiFramebuffer(false)
		.CaptureCursorOnStart()
		.EnableVSync();

	CHECK(config.fullscreen);
	CHECK(config.monitor_index == 1);
	CHECK(config.refresh_rate_hertz == 144);
	CHECK(!config.resizable);
	CHECK(!config.high_dpi_framebuffer);
	CHECK(config.capture_cursor_on_start);
	CHECK(config.vsync);
}
