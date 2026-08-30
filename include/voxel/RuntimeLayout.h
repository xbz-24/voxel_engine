#pragma once

#include <filesystem>

namespace voxel
{
	/** @addtogroup voxel_sdk_startup
	 * @{
	 */
	/** Exact native filesystem directories used instead of automatic discovery. */
	struct RuntimeLayout
	{
		std::filesystem::path asset_directory;
		std::filesystem::path vulkan_shader_directory;

		RuntimeLayout& AssetsAt(std::filesystem::path directory);
		RuntimeLayout& VulkanShadersAt(std::filesystem::path directory);
	};
	/** @} */
}
