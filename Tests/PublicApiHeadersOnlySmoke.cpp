#include <voxel/WorldTypes.h>
#include <voxel/WorldConfig.h>

#include <voxel/Assets.h>
#include <voxel/Blocks.h>
#include <voxel/Camera.h>
#include <voxel/Engine.h>
#include <voxel/EngineConfig.h>
#include <voxel/EngineConfigTypes.h>
#include <voxel/EngineRun.h>
#include <voxel/Materials.h>
#include <voxel/Runtime.h>
#include <voxel/Scene.h>
#include <voxel/Voxel.h>
#include <voxel/World.h>

#include <utility>

int PublicApiHeadersOnlySmokeCompileAnchor()
{
	const voxel::BlockPosition origin = voxel::At(
		voxel::BlockCoord(0),
		voxel::BlockCoord(63),
		voxel::BlockCoord(0));

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
			.WithSizeChunks(voxel::Chunks(2))
			.SetBlock(origin, voxel::Stone)
			.AddFloor(63, 2, voxel::Grass))
		.WithAssets(std::move(assets))
		.WithMaterials(std::move(materials))
		.WithSceneGraph(std::move(scene_graph))
		.WithCamera({ 0.0f, 70.0f, 4.0f }, { 0.0f, 64.0f, 0.0f })
		.WithVoxelRenderStyle(voxel::VoxelRenderStyle{}
			.WithSunIntensity(1.1f)
			.WithFogRange(80.0f, 300.0f))
		.UseVulkan()
		.HideDebugOverlay();
	return config.window.width;
}
