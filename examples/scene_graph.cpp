#include <voxel/Scene.h>

int main()
{
	voxel::SceneGraph scene{};
	scene.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored)
		.EnvironmentSettings(voxel::Environment{}.TimeOfDay(18.0f));

	const voxel::EntityId tower = scene.AddEntity(voxel::Entity::Named("tower")
		.At({ 4.0f, 0.0f, 4.0f })
		.Model("tower"));
	const voxel::EntityId beacon = scene.AddChild(tower, voxel::Entity::Named("beacon")
		.At({ 0.0f, 6.0f, 0.0f })
		.Model("beacon"));
	scene.Add(voxel::Light::Sun({ -1.0f, -2.0f, -1.0f }, 3.0f).CastShadows());

	return tower.IsValid() && beacon.IsValid() && scene.Validate().empty() ? 0 : 1;
}
