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

namespace
{
	template <typename Predicate>
	bool WaitForBackgroundTaskQueueCondition(Predicate predicate)
	{
		for (int attemptIndex = 0; attemptIndex < 100; attemptIndex++)
		{
			if (predicate()) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return predicate();
	}
}

TEST_CASE("background task queue runs higher priority pending work first")
{
	std::atomic_bool releaseCurrentTask = false;
	std::mutex completedTaskPrioritiesMutex;
	std::vector<int> completedTaskPriorities;
	ve::tasks::BackgroundTaskQueue queue(1);

	REQUIRE(queue.Enqueue([&releaseCurrentTask]
	{
		while (!releaseCurrentTask.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}));
	REQUIRE(WaitForBackgroundTaskQueueCondition([&queue]
	{
		return queue.Stats().startedTaskCount == 1U;
	}));
	REQUIRE(queue.Enqueue([&completedTaskPriorities, &completedTaskPrioritiesMutex]
	{
		std::lock_guard<std::mutex> lock(completedTaskPrioritiesMutex);
		completedTaskPriorities.push_back(1);
	}, ve::tasks::BackgroundTaskOptions{ .taskPriority = 1 }));
	REQUIRE(queue.Enqueue([&completedTaskPriorities, &completedTaskPrioritiesMutex]
	{
		std::lock_guard<std::mutex> lock(completedTaskPrioritiesMutex);
		completedTaskPriorities.push_back(10);
	}, ve::tasks::BackgroundTaskOptions{ .taskPriority = 10 }));

	releaseCurrentTask.store(true);
	REQUIRE(WaitForBackgroundTaskQueueCondition([&completedTaskPriorities, &completedTaskPrioritiesMutex]
	{
		std::lock_guard<std::mutex> lock(completedTaskPrioritiesMutex);
		return completedTaskPriorities.size() == 2U;
	}));

	CHECK(completedTaskPriorities[0] == 10);
	CHECK(completedTaskPriorities[1] == 1);
}

