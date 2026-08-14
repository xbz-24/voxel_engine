#include "EngineConfigTranslatorInternal.h"

#include <filesystem>
#include <system_error>

namespace voxel::detail::config_translation
{
	namespace
	{
		[[nodiscard]] std::filesystem::path CapturePath(const std::string& value)
		{
			std::error_code error;
			const std::filesystem::path path{ value };
			const std::filesystem::path absolute = std::filesystem::absolute(path, error);
			return (error ? path : absolute).lexically_normal();
		}
	}

	void ApplyRuntimeLayoutToCreateInfo(ve::engine::EngineCreateInfo& create_info,
		const EngineConfig& config)
	{
		create_info.asset_directory.reset();
		create_info.vulkan_shader_directory.reset();
		if (!config.runtime_layout) return;

		create_info.asset_directory = CapturePath(config.runtime_layout->asset_directory);
		if (!config.runtime_layout->vulkan_shader_directory.empty())
		{
			create_info.vulkan_shader_directory = CapturePath(
				config.runtime_layout->vulkan_shader_directory);
		}
	}
}
