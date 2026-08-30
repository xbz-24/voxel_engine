#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <string>
#include <vector>

TEST_CASE("public scene graph composes entities lights and environment")
{
	voxel::SceneGraph graph{};
	graph.ComposeAs(voxel::SceneGraphCompositionMode::VoxelWorldAnchored);
	const voxel::EntityId crate_id = graph.AddEntity(voxel::Entity::Named("crate")
		.At({ 1.0f, 2.0f, 3.0f })
		.Model("crate")
		.Material("glowing"));
	const voxel::EntityId glow_id = graph.AddChild(crate_id, voxel::Entity::Named("crate glow")
		.At({ 1.0f, 3.0f, 3.0f })
		.Model("crate")
		.Material("glowing"));
	graph.Add(voxel::Light::Sun({ -1.0f, -2.0f, -1.0f }, 3.0f)
			.CastShadows()
			.UseIntensityUnit(voxel::LightIntensityUnit::Lux))
		.Add(voxel::Light::Spot({ 2.0f, 4.0f, 2.0f },
			{ -1.0f, -1.0f, 0.0f }, { 1.0f, 0.8f, 0.6f, 1.0f },
			350.0f, 24.0f, 12.0f, 40.0f)
			.UseIntensityUnit(voxel::LightIntensityUnit::Candela))
		.EnvironmentSettings(voxel::Environment{}
			.TimeOfDay(18.0f)
			.Fog({ 0.4f, 0.5f, 0.7f, 1.0f }, 0.02f));

	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithSceneGraph(graph);

	REQUIRE(config.scene_graph.entities.size() == 2);
	CHECK(crate_id.IsValid());
	CHECK(glow_id.IsValid());
	CHECK(config.scene_graph.entities[0].id == crate_id);
	CHECK(config.scene_graph.entities[0].transform.position.z == doctest::Approx(3.0f));
	CHECK(config.scene_graph.entities[1].parent == crate_id);
	CHECK(config.scene_graph.composition_mode == voxel::SceneGraphCompositionMode::VoxelWorldAnchored);
	REQUIRE(config.scene_graph.FindEntity(glow_id) != nullptr);
	CHECK(config.scene_graph.FindEntity(glow_id)->name == "crate glow");
	CHECK(config.scene_graph.Validate().empty());
	REQUIRE(config.scene_graph.lights.size() == 2);
	CHECK(config.scene_graph.lights[0].shadows.enabled);
	CHECK(config.scene_graph.lights[0].intensity_unit == voxel::LightIntensityUnit::Lux);
	CHECK(config.scene_graph.lights[1].kind == voxel::LightKind::Spot);
	CHECK(config.scene_graph.lights[1].inner_cone_degrees == doctest::Approx(12.0f));
	CHECK(config.scene_graph.lights[1].outer_cone_degrees == doctest::Approx(40.0f));
	CHECK(config.scene_graph.lights[1].intensity_unit == voxel::LightIntensityUnit::Candela);
	CHECK(config.scene_graph.environment.time_of_day == doctest::Approx(18.0f));

	const std::vector<std::string> issues = config.Validate();
	CHECK(std::find(issues.begin(), issues.end(),
		"SceneGraph runtime rendering is not implemented") != issues.end());
}
