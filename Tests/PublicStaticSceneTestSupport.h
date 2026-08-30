#pragma once

#include <voxel/Voxel.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ve::tests
{
	class TemporaryStaticSceneObj
	{
	public:
		explicit TemporaryStaticSceneObj(std::string_view label,
			std::string_view extension = ".obj")
		{
			path_ = std::filesystem::absolute(std::filesystem::temp_directory_path() /
				("voxel_static_scene_" + std::string{ label } + std::string{ extension }));
			std::error_code cleanup_error;
			std::filesystem::remove(path_, cleanup_error);
			std::ofstream output(path_, std::ios::trunc);
			output << "o triangle\n"
				"v -0.5 0.0 0.0\n"
				"v 0.5 0.0 0.0\n"
				"v 0.0 1.0 0.0\n"
				"vn 0.0 0.0 1.0\n"
				"f 1//1 2//1 3//1\n";
		}

		~TemporaryStaticSceneObj()
		{
			std::error_code error;
			std::filesystem::remove(path_, error);
		}

		TemporaryStaticSceneObj(const TemporaryStaticSceneObj&) = delete;
		TemporaryStaticSceneObj& operator=(const TemporaryStaticSceneObj&) = delete;
		[[nodiscard]] const std::filesystem::path& Path() const noexcept { return path_; }

	private:
		std::filesystem::path path_;
	};

	[[nodiscard]] inline voxel::EngineConfig StaticSceneConfig(
		const std::filesystem::path& model_path,
		bool visible = true)
	{
		voxel::AssetCatalog assets{};
		assets.Model("triangle", model_path.string());
		voxel::SceneGraph scene{};
		scene.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored)
			.Add(voxel::Entity::Named("triangle").Model("triangle").Visible(visible));
		return voxel::EngineConfig::Default()
			.UseOpenGLCompatibility()
			.WithAssets(std::move(assets))
			.WithSceneGraph(std::move(scene));
	}

	[[nodiscard]] inline bool ContainsIssue(const std::vector<std::string>& issues,
		std::string_view expected)
	{
		return std::ranges::find(issues, std::string{ expected }) != issues.end();
	}
}
