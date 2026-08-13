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

TEST_CASE("rigid body world integrates gravity")
{
	ve::physics::RigidBodyWorld world({ 0.0f, -10.0f, 0.0f });
	const auto handle = world.CreateBody({ .position = { 0.0f, 10.0f, 0.0f } });

	world.Step(0.1f, 2);
	const ve::physics::RigidBody* body = world.FindBody(handle);

	REQUIRE(body != nullptr);
	CHECK(body->Position().y < 10.0f);
}

