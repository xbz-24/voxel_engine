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

TEST_CASE("block registry exposes gameplay extension metadata")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);

	CHECK(registry.BlocksMovement(ve::blocks::BlockId::Stone));
	CHECK(registry.IsTransparent(ve::blocks::BlockId::Glass));
	REQUIRE(registry.GameplayFor(ve::blocks::BlockId::Stone).drops.size() == 1U);
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Stone).drops.front().id == ve::blocks::BlockId::Stone);

	ve::blocks::BlockType water = registry.Get(ve::blocks::BlockId::Water);
	water.gameplay.collision = ve::blocks::BlockCollisionMode::None;
	water.gameplay.transparent = true;
	water.gameplay.footstep_sound = "water.step";
	water.gameplay.break_sound = "water.break";
	water.gameplay.drops.clear();
	CHECK(registry.Register(water));
	CHECK(!registry.BlocksMovement(ve::blocks::BlockId::Water));
	CHECK(registry.IsTransparent(ve::blocks::BlockId::Water));
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Water).break_sound == "water.break");
}

