#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

TEST_CASE("public static OBJ scene rejects unsupported graph composition")
{
	ve::tests::TemporaryStaticSceneObj model{ "composition" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	std::string expected;

	SUBCASE("independent overlay")
	{
		config.scene_graph.ComposeAs(voxel::SceneGraphCompositionMode::IndependentOverlay);
		expected = "Static model scenes require VoxelWorldAnchored composition";
	}
	SUBCASE("lights")
	{
		config.scene_graph.Add(voxel::Light::Point({}));
		expected = "Static model scenes do not support lights";
	}
	SUBCASE("environment")
	{
		config.scene_graph.EnvironmentSettings(voxel::Environment{}.TimeOfDay(18.0f));
		expected = "Static model scenes require the default environment";
	}
	SUBCASE("second entity")
	{
		config.scene_graph.Add(voxel::Entity::Named("second").Model("triangle"));
		expected = "Static model scenes require exactly one entity";
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(), expected));
}

TEST_CASE("public static OBJ scene rejects unsupported entity data")
{
	ve::tests::TemporaryStaticSceneObj model{ "entity" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	voxel::Entity& entity = config.scene_graph.entities.front();
	std::string expected;

	SUBCASE("missing id")
	{
		entity.id = voxel::InvalidEntityId;
		expected = "Static model scene entity must have a valid id";
	}
	SUBCASE("parent")
	{
		entity.parent = voxel::EntityId{ 9U };
		expected = "Static model scene entity must be a root";
	}
	SUBCASE("material")
	{
		entity.material = "unsupported";
		expected = "Static model scene entity material must be empty";
	}
	SUBCASE("transform")
	{
		entity.transform.position.x = 1.0f;
		expected = "Static model scene entity transform must be identity";
	}
	SUBCASE("rotation")
	{
		entity.transform.rotation.y = 30.0f;
		expected = "Static model scene entity transform must be identity";
	}
	SUBCASE("scale")
	{
		entity.transform.scale.z = 2.0f;
		expected = "Static model scene entity transform must be identity";
	}
	SUBCASE("wrong model reference")
	{
		entity.model = "other";
		expected = "Static model scene entity must reference the registered model";
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(), expected));
}

TEST_CASE("public static OBJ scene rejects public material libraries")
{
	ve::tests::TemporaryStaticSceneObj model{ "material_library" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	config.materials.Add(voxel::Material::Named("unused"));

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"Static model scenes do not support MaterialLibrary"));
	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"MaterialLibrary runtime binding is not implemented"));
}
