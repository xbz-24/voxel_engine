#include <voxel/Voxel.h>

#include <utility>

int PublicApiHeadersOnlySmokeCompileAnchor()
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("assets")
		.Texture("grass", "assets/grass.png")
		.Model("crate", voxel::AssetSource::Archive("assets/models.pack", "crate.obj"));

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("grass-material")
		.Texture("grass")
		.Roughness(0.8f));

	voxel::SceneGraph scene_graph{};
	const voxel::EntityId crate_id = scene_graph.AddEntity(voxel::Entity::Named("crate")
		.At({ 1.0f, 2.0f, 3.0f })
		.Model("crate")
		.Material("grass-material"));
	(void)scene_graph.AddChild(crate_id, voxel::Entity::Named("crate-child")
		.ChildOf(crate_id)
		.Visible(false));
	scene_graph.Add(voxel::Light::Sun({ 0.0f, -1.0f, 0.0f }));

	voxel::EngineConfig config{};
	config.WithWindow(voxel::Window("Headers Only", 320, 200))
		.WithWorld(voxel::Scene()
			.WithSizeChunks(2)
			.AddFloor(63, 2, voxel::Grass))
		.WithAssets(std::move(assets))
		.WithMaterials(std::move(materials))
		.WithSceneGraph(std::move(scene_graph))
		.WithCamera({ 0.0f, 70.0f, 4.0f }, { 0.0f, 64.0f, 0.0f })
		.UseVulkan()
		.HideDebugOverlay();
	return config.window.width;
}
