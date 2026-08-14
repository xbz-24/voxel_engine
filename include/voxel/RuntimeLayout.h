#pragma once

#include <string>

namespace voxel
{
	/** @addtogroup voxel_sdk_startup
	 * @{
	 */
	/** Exact runtime resource directories used instead of automatic discovery. */
	struct RuntimeLayout
	{
		std::string asset_directory;
		std::string vulkan_shader_directory;

		RuntimeLayout& AssetsAt(std::string directory);
		RuntimeLayout& VulkanShadersAt(std::string directory);
	};
	/** @} */
}
