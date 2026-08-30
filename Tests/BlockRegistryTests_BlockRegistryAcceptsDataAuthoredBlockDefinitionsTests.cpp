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

TEST_CASE("block registry accepts data-authored block definitions")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);

	ve::blocks::DataBlockDefinition authored_bricks{};
	authored_bricks.id = ve::blocks::BlockId::Bricks;
	authored_bricks.name = "Authored Bricks";
	authored_bricks.is_solid = true;
	authored_bricks.face_texture_files.fill("authored_bricks.png");

	CHECK(registry.RegisterDataDefinition(
		authored_bricks,
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly));
	CHECK(registry.Get(ve::blocks::BlockId::Bricks).name == "Authored Bricks");
	CHECK(registry.OccludesNeighborFaces(ve::blocks::BlockId::Bricks));
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Bricks).drops.front().id == ve::blocks::BlockId::Bricks);
	CHECK(registry.TextureFor(ve::blocks::BlockId::Bricks, ve::blocks::BlockFace::Top) == ve::rendering::kInvalidTextureHandle);

	const ve::blocks::DataBlockDefinition invalid_count{
		ve::blocks::BlockId::Count,
		"Invalid",
		false,
		{},
		{},
		{}
	};
	const ve::blocks::DataBlockDefinition invalid_name{
		ve::blocks::BlockId::Stone,
		"",
		true,
		{},
		{},
		{}
	};
	const std::array definitions{ authored_bricks, invalid_count, invalid_name };

	CHECK(registry.RegisterDataDefinitions(
		definitions,
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly) == 1U);
}
