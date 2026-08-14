#include "EngineRuntime.h"

#include "Logger.h"

#include <filesystem>
#include <optional>

namespace ve::engine
{
	void EngineRuntime::PrepareAssetsAndLogging()
	{
		ve::assets::AssetPathResolveOptions options;
		options.explicit_asset_directory = configuration_.asset_directory;
		options.explicit_vulkan_shader_directory = configuration_.vulkan_shader_directory;
		options.search_roots = configuration_.asset_search_roots;
		asset_paths_ = ve::assets::Resolve(options);
		ve::log::SetCallback(configuration_.on_log);
		std::optional<std::filesystem::path> file_output_path;
		if (configuration_.logging.file_output_enabled)
		{
			file_output_path = configuration_.logging.file_output_path.empty()
				? asset_paths_.rootDirectory / "logs/engine.log"
				: configuration_.logging.file_output_path;
		}
		ve::log::ApplyConfiguration(ve::log::LoggerConfiguration{
			configuration_.logging.minimum_level,
			configuration_.logging.console_enabled,
			file_output_path
		});
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime started");
	}
}
