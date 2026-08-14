#include "EngineConfiguration.h"
#include "EngineConfigurationValidationDetail.h"

#include "VulkanShaderPathDiscovery.h"
#include "RenderBackendSelector.h"

#include <filesystem>

namespace ve::engine::configuration_validation
{
	namespace
	{
		void CapturePath(std::optional<std::filesystem::path>& value)
		{
			if (!value || value->empty()) return;
			std::error_code error;
			const std::filesystem::path absolute = std::filesystem::absolute(*value, error);
			*value = (error ? *value : absolute).lexically_normal();
		}
	}

	void CaptureRuntimeLayoutPaths(EngineCreateInfo& create_info)
	{
		CapturePath(create_info.asset_directory);
		CapturePath(create_info.vulkan_shader_directory);
	}

	void ValidateRuntimeLayout(const EngineCreateInfo& create_info,
		std::vector<std::string>& issues)
	{
		if (create_info.asset_directory)
		{
			std::error_code error;
			if (create_info.asset_directory->empty())
				issues.push_back("asset_directory must not be empty when explicitly configured");
			else if (!std::filesystem::is_directory(*create_info.asset_directory, error))
				issues.push_back("asset_directory must name an existing directory");
		}

		if (!create_info.vulkan_shader_directory)
		{
			const ve::rendering::RenderBackendSelection selected =
				ve::rendering::RenderBackendSelector::Select(create_info.render_backend);
			if (!selected.headless && selected.api == ve::rendering::GraphicsApi::Vulkan &&
				create_info.asset_directory)
			{
				issues.push_back("vulkan_shader_directory is required for an explicit Vulkan runtime layout");
			}
			return;
		}
		if (!ve::assets::IsCompleteVulkanShaderDirectory(*create_info.vulkan_shader_directory))
			issues.push_back(
				"vulkan_shader_directory must contain the complete Vulkan shader bundle "
				"with recognizable SPIR-V headers");
	}
}
