#include <voxel/Voxel.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

namespace
{
	std::filesystem::path CreateTriangleModel()
	{
		const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
		const std::filesystem::path directory = std::filesystem::temp_directory_path() /
			("voxel_static_scene_" + std::to_string(unique));
		std::filesystem::create_directories(directory);
		const std::filesystem::path model_path = directory / "triangle.obj";
		std::ofstream model{ model_path };
		model << "o triangle\n"
			"v -0.5 0.0 0.0\n"
			"v 0.5 0.0 0.0\n"
			"v 0.0 1.0 0.0\n"
			"vn 0.0 0.0 1.0\n"
			"f 1//1 2//1 3//1\n";
		return std::filesystem::absolute(model_path);
	}

	voxel::EngineConfig CreateConfig(const std::filesystem::path& assets,
		const std::filesystem::path& model_path)
	{
		voxel::AssetCatalog catalog{};
		catalog.Model("triangle", model_path.string());
		voxel::SceneGraph scene{};
		scene.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored)
			.Add(voxel::Entity::Named("triangle").Model("triangle"));
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
			.OnUpdate([frames = 0](voxel::FrameContext& frame) mutable {
				if (++frames >= 3) frame.commands.RequestClose();
			});
	}
}

int main(int argc, char** argv)
{
	if (argc != 2 || argv == nullptr || argv[1] == nullptr) return 2;
	const std::filesystem::path model_path = CreateTriangleModel();
	const int result = voxel::Run(CreateConfig(std::filesystem::path{ argv[1] }, model_path));
	std::error_code error;
	std::filesystem::remove_all(model_path.parent_path(), error);
	return result;
}
