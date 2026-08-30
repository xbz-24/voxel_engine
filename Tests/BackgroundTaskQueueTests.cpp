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

TEST_CASE("background task queue records worker telemetry")
{
	std::atomic_bool completed = false;
	ve::tasks::BackgroundTaskQueue queue(1);

	REQUIRE(queue.Enqueue([&completed]
	{
		completed.store(true);
	}));

	for (int attemptIndex = 0; attemptIndex < 100 && !completed.load(); attemptIndex++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	const ve::tasks::BackgroundTaskQueueStats stats = queue.Stats();
	CHECK(completed.load());
	CHECK(stats.acceptedTaskCount == 1U);
	CHECK(stats.startedTaskCount == 1U);
	CHECK(stats.completedTaskCount == 1U);
	CHECK(stats.pendingTaskCount == 0U);
	CHECK(stats.longestQueueLatency.count() >= 0);
	CHECK(stats.longestExecutionTime.count() >= 0);
}

TEST_CASE("background task queue rejects empty tasks before workers run them")
{
	ve::tasks::BackgroundTaskQueue queue(1);
	ve::tasks::BackgroundTask emptyTask;

	CHECK(!queue.Enqueue(emptyTask));
	const ve::tasks::BackgroundTaskQueueStats stats = queue.Stats();
	CHECK(stats.acceptedTaskCount == 0U);
	CHECK(stats.rejectedTaskCount == 1U);
	CHECK(stats.pendingTaskCount == 0U);
}
