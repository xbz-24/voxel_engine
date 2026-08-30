#include <doctest/doctest.h>

#include "BackgroundTaskQueue.h"
#include "BlockRegistry.h"
#include "EcsWorld.h"
#include "FabrikInverseKinematicsSolver.h"
#include "Logger.h"
#include "MonteCarloPathTracer.h"
#include "NavierStokesFluidSimulation.h"
#include "PhysicsBounds.h"
#include "RigidBodyWorld.h"
#include "SimdFloat4.h"
#include "Engine.h"
#include "GameplayInput.h"
#include "ModelAssetLibrary.h"
#include "MouseLookInput.h"
#include "PhysicsBvh.h"
#include "RuntimeInputRouter.h"
#include "TestTypeHelpers.h"

#include <glm/geometric.hpp>

#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <vector>

TEST_CASE("ecs world stores components behind generation-safe entity handles")
{
	struct PositionComponent
	{
		glm::vec3 value{ 0.0f };
	};

	ve::ecs::EcsWorld world;
	const ve::ecs::Entity entity = world.CreateEntity();
	PositionComponent* position = world.EmplaceComponent<PositionComponent>(
		entity,
		PositionComponent{ glm::vec3{ 1.0f, 2.0f, 3.0f } });

	REQUIRE(position != nullptr);
	CHECK(world.HasComponent<PositionComponent>(entity));
	CHECK(world.ComponentCount<PositionComponent>() == 1U);
	CHECK(world.FindComponent<PositionComponent>(entity)->value.y == doctest::Approx(2.0f));

	CHECK(world.DestroyEntity(entity));
	CHECK(!world.HasComponent<PositionComponent>(entity));
	CHECK(world.ComponentCount<PositionComponent>() == 0U);

	const ve::ecs::Entity recycled_entity = world.CreateEntity();
	CHECK(recycled_entity.id == entity.id);
	CHECK(recycled_entity.generation != entity.generation);
	CHECK(world.FindComponent<PositionComponent>(entity) == nullptr);
}

