#pragma once

#include "voxel/EngineConfig.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace voxel::detail::static_scene
{
	struct StaticModelSceneSelection
	{
		std::filesystem::path model_path;
		bool visible = true;
	};

	struct StaticModelSceneAnalysis
	{
		bool has_asset_content = false;
		bool has_scene_content = false;
		std::optional<StaticModelSceneSelection> runtime_candidate;
		std::optional<StaticModelSceneSelection> supported_scene;
		std::vector<std::string> issues;
	};

	[[nodiscard]] StaticModelSceneAnalysis AnalyzeStaticModelScene(
		const EngineConfig& config);
}
