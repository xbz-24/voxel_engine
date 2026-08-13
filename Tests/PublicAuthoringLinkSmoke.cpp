#include <voxel/Assets.h>
#include <voxel/EngineConfig.h>
#include <voxel/MaterialLibrary.h>
#include <voxel/SceneGraph.h>
#include <voxel/WorldConfig.h>

#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

int main()
{
	voxel::AssetCatalog assets{};
	assets.Texture("grass", voxel::AssetSource::Embedded({ 1U, 2U, 3U }));
	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("ground").Texture("grass"));
	voxel::SceneGraph scene{};
	scene.Add(voxel::Entity::Named("marker").Material("ground"));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(std::move(assets))
		.WithMaterials(std::move(materials))
		.WithSceneGraph(std::move(scene))
		.WithWorld(voxel::World(3).AddFloor(0, 2, voxel::Grass));

	const std::vector<std::string> issues = config.Validate();
	const auto has_issue = [&issues](const std::string& expected)
	{
		return std::find(issues.begin(), issues.end(), expected) != issues.end();
	};
	return config.world.edits.size() == 1 &&
		has_issue("AssetCatalog runtime loading is not implemented") &&
		has_issue("MaterialLibrary runtime binding is not implemented") &&
		has_issue("SceneGraph runtime rendering is not implemented") ? 0 : 1;
}
