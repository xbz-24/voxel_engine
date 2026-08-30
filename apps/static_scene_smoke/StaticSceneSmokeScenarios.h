#pragma once

#include <filesystem>

namespace static_scene_smoke
{
	[[nodiscard]] int RunBoundedScene(
		const std::filesystem::path& assets,
		const std::filesystem::path& model_path);

	[[nodiscard]] int RunExceptionRecovery(
		const std::filesystem::path& assets,
		const std::filesystem::path& model_path);

	[[nodiscard]] int RunReentrantLifecycle(
		const std::filesystem::path& assets,
		const std::filesystem::path& model_path);
}
