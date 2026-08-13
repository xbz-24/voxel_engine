#include "EngineConfiguration.h"
#include "EngineConfigurationValidationDetail.h"

namespace ve::engine::configuration_validation
{
	namespace
	{
		[[nodiscard]] bool IsKnownGraphicsApi(ve::rendering::GraphicsApi api) noexcept
		{
			switch (api)
			{
			case ve::rendering::GraphicsApi::Vulkan:
			case ve::rendering::GraphicsApi::OpenGLCompatibility:
			case ve::rendering::GraphicsApi::DirectX12:
				return true;
			default:
				return false;
			}
		}

		[[nodiscard]] bool IsKnownSelectionPolicy(
			ve::rendering::RenderBackendSelectionPolicy policy) noexcept
		{
			switch (policy)
			{
			case ve::rendering::RenderBackendSelectionPolicy::ExactBackendOnly:
			case ve::rendering::RenderBackendSelectionPolicy::BestAvailable:
			case ve::rendering::RenderBackendSelectionPolicy::Headless:
				return true;
			default:
				return false;
			}
		}
	}

	void ValidateWindowAndBackend(
		const EngineCreateInfo& create_info,
		std::vector<std::string>& issues)
	{
		if (create_info.window.width <= 0)
			issues.push_back("window.width must be greater than zero");
		if (create_info.window.height <= 0)
			issues.push_back("window.height must be greater than zero");
		if (create_info.window.display_index < 0)
			issues.push_back("window.display_index must be zero or greater");
		if (create_info.window.refresh_rate_hertz < 0)
			issues.push_back("window.refresh_rate_hertz must be zero or greater");
		if (!IsKnownGraphicsApi(create_info.render_backend.preferred_api))
			issues.push_back("render_backend.preferred_api is not a known graphics api");
		if (!IsKnownSelectionPolicy(create_info.render_backend.selection_policy))
			issues.push_back("render_backend.selection_policy is not a known selection policy");
	}
}
