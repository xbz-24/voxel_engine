#include "EngineConfiguration.h"
#include "EngineConfigurationValidationDetail.h"

#include "RenderBackendSelector.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <system_error>

namespace ve::engine::configuration_validation
{
	namespace
	{
		[[nodiscard]] bool HasObjExtension(const std::filesystem::path& path)
		{
			std::string extension = path.extension().string();
			std::ranges::transform(extension, extension.begin(), [](unsigned char value) {
				return static_cast<char>(std::tolower(value));
			});
			return extension == ".obj";
		}
	}

	void ValidateStaticModelScene(const EngineCreateInfo& create_info,
		std::vector<std::string>& issues)
	{
		if (!create_info.static_model_scene) return;
		const ve::rendering::RenderBackendSelection selected =
			ve::rendering::RenderBackendSelector::Select(create_info.render_backend);
		if (selected.headless || selected.api != ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			issues.push_back(
				"static_model_scene requires the selected graphics api to be OpenGLCompatibility");
		}
		if (!create_info.asset_search_roots.empty())
			issues.push_back("static_model_scene does not support asset_search_roots");

		const std::filesystem::path& path = create_info.static_model_scene->model_path;
		if (path.empty())
		{
			issues.push_back("static_model_scene.model_path must not be empty");
			return;
		}
		if (!path.is_absolute())
			issues.push_back("static_model_scene.model_path must be absolute");
		if (!HasObjExtension(path))
			issues.push_back("static_model_scene.model_path must have a .obj extension");
		std::error_code error;
		if (!std::filesystem::is_regular_file(path, error))
			issues.push_back("static_model_scene.model_path must name an existing regular file");
	}
}
