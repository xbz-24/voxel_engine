#include <doctest/doctest.h>

#include "BackgroundTaskQueue.h"

#include <atomic>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

namespace
{
	bool WaitForFinishedTasks(ve::tasks::BackgroundTaskQueue& queue)
	{
		const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
		do
		{
			const ve::tasks::BackgroundTaskQueueStats stats = queue.Stats();
			if (stats.completedTaskCount == 1U && stats.failedTaskCount == 1U) return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} while (std::chrono::steady_clock::now() < deadline);
		return false;
	}
}

TEST_CASE("background task queue contains task and failure-handler exceptions")
{
	std::atomic_bool failureObserved = false;
	std::atomic_int failureHandlerCalls = 0;
	std::atomic_bool nextTaskRan = false;
	ve::tasks::BackgroundTaskQueue queue(1);
	ve::tasks::BackgroundTaskOptions options;
	options.failureHandler = [&failureObserved, &failureHandlerCalls](std::exception_ptr failure)
	{
		failureObserved.store(static_cast<bool>(failure));
		failureHandlerCalls.fetch_add(1);
		throw std::runtime_error("failure handler also failed");
	};

	REQUIRE(queue.Enqueue([] { throw std::runtime_error("task failed"); }, options));
	REQUIRE(queue.Enqueue([&nextTaskRan] { nextTaskRan.store(true); }));
	REQUIRE(WaitForFinishedTasks(queue));
	const ve::tasks::BackgroundTaskQueueStats stats = queue.Stats();

	CHECK(failureObserved.load());
	CHECK(failureHandlerCalls.load() == 1);
	CHECK(nextTaskRan.load());
	CHECK(stats.acceptedTaskCount == 2U);
	CHECK(stats.startedTaskCount == 2U);
	CHECK(stats.completedTaskCount == 1U);
	CHECK(stats.failedTaskCount == 1U);
	CHECK(stats.pendingTaskCount == 0U);
	CHECK(stats.startedTaskCount == stats.completedTaskCount + stats.failedTaskCount);
}
