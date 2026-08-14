#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <string>
#include <vector>

namespace
{
	[[nodiscard]] bool HasCycleIssue(const std::vector<std::string>& issues)
	{
		return std::ranges::any_of(issues, [](const std::string& issue) {
			return issue.ends_with(" participates in a parent cycle");
		});
	}
}

TEST_CASE("public scene graph validation reports two-node cycle members by id")
{
	voxel::SceneGraph graph{};
	(void)graph.AddEntity(voxel::Entity::Named("second").WithId({ 2U }).ChildOf({ 1U }));
	(void)graph.AddEntity(voxel::Entity::Named("first").WithId({ 1U }).ChildOf({ 2U }));

	const std::vector<std::string> issues = graph.Validate();
	REQUIRE(issues.size() == 2);
	CHECK(issues[0] == "scene entity 'first' participates in a parent cycle");
	CHECK(issues[1] == "scene entity 'second' participates in a parent cycle");
}

TEST_CASE("public scene graph validation finds a three-node cycle in another component")
{
	voxel::SceneGraph graph{};
	(void)graph.AddEntity(voxel::Entity::Named("root").WithId({ 1U }));
	(void)graph.AddEntity(voxel::Entity::Named("third").WithId({ 30U }).ChildOf({ 10U }));
	(void)graph.AddEntity(voxel::Entity::Named("first").WithId({ 10U }).ChildOf({ 20U }));
	(void)graph.AddEntity(voxel::Entity::Named("second").WithId({ 20U }).ChildOf({ 30U }));

	const std::vector<std::string> issues = graph.Validate();
	REQUIRE(issues.size() == 3);
	CHECK(issues[0] == "scene entity 'first' participates in a parent cycle");
	CHECK(issues[1] == "scene entity 'second' participates in a parent cycle");
	CHECK(issues[2] == "scene entity 'third' participates in a parent cycle");
}

TEST_CASE("public scene graph cycle diagnostics exclude an incoming tail")
{
	voxel::SceneGraph graph{};
	(void)graph.AddEntity(voxel::Entity::Named("tail").WithId({ 1U }).ChildOf({ 2U }));
	(void)graph.AddEntity(voxel::Entity::Named("first").WithId({ 2U }).ChildOf({ 3U }));
	(void)graph.AddEntity(voxel::Entity::Named("second").WithId({ 3U }).ChildOf({ 2U }));

	const std::vector<std::string> issues = graph.Validate();
	REQUIRE(issues.size() == 2);
	CHECK(issues[0] == "scene entity 'first' participates in a parent cycle");
	CHECK(issues[1] == "scene entity 'second' participates in a parent cycle");
}

TEST_CASE("public scene graph cycle validation ignores duplicated parent ids")
{
	voxel::SceneGraph graph{};
	(void)graph.AddEntity(voxel::Entity::Named("child").WithId({ 1U }).ChildOf({ 2U }));
	(void)graph.AddEntity(voxel::Entity::Named("first duplicate").WithId({ 2U }).ChildOf({ 1U }));
	(void)graph.AddEntity(voxel::Entity::Named("second duplicate").WithId({ 2U }));

	const std::vector<std::string> issues = graph.Validate();
	CHECK_FALSE(HasCycleIssue(issues));
	CHECK(std::ranges::find(issues,
		"scene entity 'second duplicate' has a duplicated id") != issues.end());
}

TEST_CASE("public scene graph validation accepts a root child grandchild chain")
{
	voxel::SceneGraph graph{};
	const voxel::EntityId root = graph.AddEntity(voxel::Entity::Named("root"));
	const voxel::EntityId child = graph.AddChild(root, voxel::Entity::Named("child"));
	(void)graph.AddChild(child, voxel::Entity::Named("grandchild"));

	CHECK(graph.Validate().empty());
}

TEST_CASE("public scene graph validation reports disjoint parent cycles")
{
	voxel::SceneGraph graph{};
	(void)graph.AddEntity(voxel::Entity::Named("one").WithId({ 1U }).ChildOf({ 2U }));
	(void)graph.AddEntity(voxel::Entity::Named("two").WithId({ 2U }).ChildOf({ 1U }));
	(void)graph.AddEntity(voxel::Entity::Named("three").WithId({ 3U }).ChildOf({ 4U }));
	(void)graph.AddEntity(voxel::Entity::Named("four").WithId({ 4U }).ChildOf({ 3U }));

	const std::vector<std::string> issues = graph.Validate();
	REQUIRE(issues.size() == 4);
	CHECK(issues[0].find("'one'") != std::string::npos);
	CHECK(issues[1].find("'two'") != std::string::npos);
	CHECK(issues[2].find("'three'") != std::string::npos);
	CHECK(issues[3].find("'four'") != std::string::npos);
}
