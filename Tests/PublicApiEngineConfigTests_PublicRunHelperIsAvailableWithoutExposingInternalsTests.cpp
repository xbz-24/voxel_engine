#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public run helper is available without exposing internals")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow("Window API Smoke", 800, 600)
		.UseOpenGLCompatibility()
		.HideDebugOverlay();

	CHECK(!config.window.fullscreen);
	CHECK(config.window.title == "Window API Smoke");
	CHECK(config.window.width == 800);
	CHECK(config.window.height == 600);
	CHECK(config.graphics_api == voxel::GraphicsApi::OpenGLCompatibility);
	CHECK(!config.show_debug_overlay);
}

