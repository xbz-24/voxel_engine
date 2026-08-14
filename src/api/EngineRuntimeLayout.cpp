#include "voxel/RuntimeLayout.h"

#include <utility>

namespace voxel
{
	RuntimeLayout& RuntimeLayout::AssetsAt(std::string directory)
	{
		asset_directory = std::move(directory);
		return *this;
	}

	RuntimeLayout& RuntimeLayout::VulkanShadersAt(std::string directory)
	{
		vulkan_shader_directory = std::move(directory);
		return *this;
	}
}
