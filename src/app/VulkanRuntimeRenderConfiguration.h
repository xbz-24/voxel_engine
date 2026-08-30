#pragma once

#include "VoxelRenderStyle.h"

#include <filesystem>

namespace ve::engine
{
	struct VulkanRuntimeRenderConfiguration
	{
		std::filesystem::path block_texture_directory;
		std::filesystem::path shader_directory;
		ve::rendering::VoxelRenderStyle render_style{};
		bool enable_imgui_overlay = true;
	};
}
