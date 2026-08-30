#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public scene graph validation reports invalid hierarchy")
{
	voxel::SceneGraph graph{};
	graph.entities.push_back(voxel::Entity::Named("missing id"));
	(void)graph.AddEntity(voxel::Entity::Named("root").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("duplicate").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("self parent")
		.WithId(voxel::EntityId{ 8U })
		.ChildOf(voxel::EntityId{ 8U }));
	(void)graph.AddEntity(voxel::Entity::Named("missing parent")
		.WithId(voxel::EntityId{ 9U })
		.ChildOf(voxel::EntityId{ 100U }));

	const std::vector<std::string> issues = graph.Validate();

	CHECK(graph.FindEntity(voxel::InvalidEntityId) == nullptr);
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing id' must have a stable id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'duplicate' has a duplicated id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'self parent' cannot be parented to itself") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing parent' references a missing parent id") != issues.end());
}

