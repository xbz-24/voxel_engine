#include "voxel/RuntimeLayout.h"

#include <utility>

namespace voxel
{
	RuntimeLayout& RuntimeLayout::AssetsAt(std::filesystem::path directory)
	{
		asset_directory = std::move(directory);
		return *this;
	}

	RuntimeLayout& RuntimeLayout::VulkanShadersAt(std::filesystem::path directory)
	{
		vulkan_shader_directory = std::move(directory);
		return *this;
	}
}
