#include "StaticSceneSmokeSupport.h"

#include <chrono>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

namespace static_scene_smoke
{
	TriangleModelFixture::TriangleModelFixture()
	{
		const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
		const std::filesystem::path directory = std::filesystem::temp_directory_path() /
			("voxel_static_scene_" + std::to_string(unique));
		std::filesystem::create_directories(directory);
		path_ = std::filesystem::absolute(directory / "triangle.obj");
		std::ofstream model{ path_ };
		model << "o triangle\n"
			"v -0.5 0.0 0.0\n"
			"v 0.5 0.0 0.0\n"
			"v 0.0 1.0 0.0\n"
			"vn 0.0 0.0 1.0\n"
			"f 1//1 2//1 3//1\n";
	}

	TriangleModelFixture::~TriangleModelFixture()
	{
		std::error_code error;
		std::filesystem::remove_all(path_.parent_path(), error);
	}

	const std::filesystem::path& TriangleModelFixture::Path() const noexcept
	{
		return path_;
	}

	voxel::EngineConfig CreateConfig(const std::filesystem::path& assets,
		const std::filesystem::path& model_path, voxel::UpdateCallback on_update,
		voxel::LogCallback on_log)
	{
		voxel::AssetCatalog catalog{};
		catalog.Model("triangle", model_path.string());
		voxel::SceneGraph scene{};
		scene.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored)
			.Add(voxel::Entity::Named("triangle")
				.At({ 0.25f, 0.0f, 0.0f })
				.Model("triangle"));
		voxel::RuntimeLayout layout{};
		layout.AssetsAt(assets);
		return voxel::EngineConfig::Default()
			.UseOpenGLCompatibility()
			.WithRuntimeLayout(std::move(layout))
			.WithAssets(std::move(catalog))
			.WithSceneGraph(std::move(scene))
			.WithCamera({ 0.0f, 0.0f, 2.0f }, { 0.0f, 0.0f, 0.0f })
			.WithWorldSizeChunks(2)
			.WithRenderDistanceChunks(2)
			.DisableSettingsMenu()
			.HideDebugOverlay()
			.OnUpdate(std::move(on_update))
			.OnLog(std::move(on_log));
	}
}
