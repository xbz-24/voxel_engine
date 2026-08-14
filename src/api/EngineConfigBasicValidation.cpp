#include "EngineConfigValidationInternal.h"

namespace voxel::detail::config_validation
{
	void ValidateWindowAndWorldConfiguration(const EngineConfig& config,
		std::vector<std::string>& issues)
	{
		if (config.window.width <= 0)
		{
			issues.push_back("window.width must be greater than zero");
		}
		if (config.window.height <= 0)
		{
			issues.push_back("window.height must be greater than zero");
		}
		if (config.window.monitor_index < 0)
		{
			issues.push_back("window.monitor_index must be zero or greater");
		}
		if (config.window.refresh_rate_hertz < 0)
		{
			issues.push_back("window.refresh_rate_hertz must be zero or greater");
		}
		if (config.world.size_chunks <= 0)
		{
			issues.push_back("world.size_chunks must be greater than zero");
		}
		if (config.render_distance_chunks < 0)
		{
			issues.push_back("render_distance_chunks must be zero or greater");
		}
		if (config.logging.file_output_enabled && config.logging.file_output_path.empty())
		{
			issues.push_back(
				"logging.file_output_path must not be empty when file output is enabled");
		}
	}

	void ValidateGraphicsBackendSupport(const EngineConfig& config,
		std::vector<std::string>& issues)
	{
		if (config.graphics_api == GraphicsApi::DirectX12)
		{
			issues.push_back("DirectX12 backend is declared but not implemented");
		}
	}
}
