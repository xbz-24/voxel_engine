#pragma once

#include <string>
#include <vector>

namespace ve::engine
{
	struct EngineCreateInfo;
}

namespace ve::rendering
{
	struct VoxelRenderStyle;
}

namespace ve::engine::configuration_validation
{
	void ValidateWindowAndBackend(
		const EngineCreateInfo& create_info,
		std::vector<std::string>& issues);
	void ValidateRuntimeLayout(
		const EngineCreateInfo& create_info,
		std::vector<std::string>& issues);
	void CaptureRuntimeLayoutPaths(EngineCreateInfo& create_info);
	void ValidateVoxelRenderStyle(
		const ve::rendering::VoxelRenderStyle& style,
		std::vector<std::string>& issues);
	void ValidateVoxelSurfaceStyle(
		const ve::rendering::VoxelRenderStyle& style,
		std::vector<std::string>& issues);
	void ValidateWorldAndCamera(
		const EngineCreateInfo& create_info,
		std::vector<std::string>& issues);
}
