#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "EcsWorld.h"
#include "FabrikInverseKinematicsSolver.h"
#include "PhysicsBounds.h"
#include "RigidBodyWorld.h"

#include <glm/geometric.hpp>

#include <array>

TEST_CASE("ecs registry creates and destroys alive entities")
{
	ve::ecs::EcsWorld world;
	const ve::ecs::Entity entity = world.CreateEntity();

	CHECK(entity.IsValid());
	CHECK(world.IsAlive(entity));
	CHECK(world.AliveCount() == 1);
	CHECK(world.DestroyEntity(entity));
	CHECK(world.AliveCount() == 0);
}

TEST_CASE("physics bounds report overlapping boxes")
{
	const ve::physics::Aabb left{ glm::vec3{ 0.0f }, glm::vec3{ 2.0f } };
	const ve::physics::Aabb right{ glm::vec3{ 1.0f }, glm::vec3{ 3.0f } };

	CHECK(left.Intersects(right));
	CHECK(left.Contains({ 1.0f, 1.0f, 1.0f }));
}

TEST_CASE("rigid body world integrates gravity")
{
	ve::physics::RigidBodyWorld world({ 0.0f, -10.0f, 0.0f });
	const auto handle = world.CreateBody({ .position = { 0.0f, 10.0f, 0.0f } });

	world.Step(0.1f, 2);
	const ve::physics::RigidBody* body = world.FindBody(handle);

	REQUIRE(body != nullptr);
	CHECK(body->Position().y < 10.0f);
}

TEST_CASE("fabrik inverse kinematics moves end effector toward target")
{
	std::array<glm::vec3, 3> joints{ glm::vec3{ 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 2.0f, 0.0f, 0.0f } };
	ve::animation::InverseKinematicsChain chain;
	chain.SetJointPositions(joints);

	const ve::animation::FabrikInverseKinematicsSolver solver;
	const bool solved = solver.Solve(chain, { 1.0f, 1.0f, 0.0f });

	CHECK(solved);
	CHECK(glm::distance(chain.EndEffectorPosition(), glm::vec3{ 1.0f, 1.0f, 0.0f }) < 0.05f);
}
