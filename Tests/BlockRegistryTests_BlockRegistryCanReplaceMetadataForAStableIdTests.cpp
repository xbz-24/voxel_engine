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

TEST_CASE("block registry can replace metadata for a stable id")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	ve::blocks::BlockType ghost_glass = registry.Get(ve::blocks::BlockId::Glass);
	ghost_glass.name = "Ghost Glass";
	ghost_glass.isSolid = true;

	CHECK(registry.Register(ghost_glass));
	CHECK(registry.Get(ve::blocks::BlockId::Glass).name == "Ghost Glass");
	CHECK(registry.IsSolid(ve::blocks::BlockId::Glass));
	CHECK(!registry.Register(ve::blocks::BlockType{
		ve::blocks::BlockId::Count,
		"Invalid",
		false,
		{},
		{},
		{}
	}));
}

