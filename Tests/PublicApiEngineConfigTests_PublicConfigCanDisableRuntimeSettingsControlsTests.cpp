#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public config can disable runtime settings controls")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.DisableSettingsMenu();

	CHECK(!config.enable_settings_menu);
	CHECK(!config.RuntimeTuning().enable_settings_menu);
}

