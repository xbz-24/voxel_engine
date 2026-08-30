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

TEST_CASE("physics bvh exposes parent depth metadata")
{
	const std::array<ve::physics::PhysicsProxy, 4> proxies{
		ve::physics::PhysicsProxy{ 11, { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 22, { { 3.0f, 0.0f, 0.0f }, { 4.0f, 1.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 33, { { 0.0f, 3.0f, 0.0f }, { 1.0f, 4.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 44, { { 3.0f, 3.0f, 0.0f }, { 4.0f, 4.0f, 1.0f } } }
	};
	ve::physics::PhysicsBvh bvh;

	bvh.Build(proxies);

	CHECK(!bvh.IsEmpty());
	CHECK(bvh.NodeCount() == 7);
	const ve::physics::PhysicsBvh::NodeDebugInfo root = bvh.DebugNode(0);
	CHECK(root.parent == -1);
	CHECK(root.depth == 0);
	CHECK(!root.is_leaf);

	for (ve::core::Index node_index = 1; node_index < bvh.NodeCount(); ++node_index)
	{
		const ve::physics::PhysicsBvh::NodeDebugInfo node = bvh.DebugNode(node_index);
		REQUIRE(node.parent >= 0);
		const ve::physics::PhysicsBvh::NodeDebugInfo parent = bvh.DebugNode(ve::core::ToIndex(node.parent));
		CHECK(node.depth == parent.depth + 1);
	}

	ve::core::DynamicArray<unsigned int> overlapped_ids;
	bvh.QueryOverlaps({ { -0.5f, -0.5f, -0.5f }, { 1.5f, 1.5f, 1.5f } }, overlapped_ids);
	REQUIRE(overlapped_ids.size() == 1);
	CHECK(overlapped_ids.front() == 11);

	CHECK(bvh.UpdateProxyBounds(22, { { 0.25f, 0.25f, 0.25f }, { 0.75f, 0.75f, 0.75f } }));
	overlapped_ids.clear();
	bvh.QueryOverlaps({ { -0.5f, -0.5f, -0.5f }, { 1.5f, 1.5f, 1.5f } }, overlapped_ids);
	CHECK(overlapped_ids.size() == 2U);
}

