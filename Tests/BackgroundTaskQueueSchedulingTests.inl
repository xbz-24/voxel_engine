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

TEST_CASE("background task queue cancels pending work by cancellation id")
{
	constexpr std::uint64_t meshBuildCancellationId = 77;
	std::atomic_bool releaseCurrentTask = false;
	std::atomic_bool canceledTaskRan = false;
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
	REQUIRE(queue.Enqueue([&canceledTaskRan]
	{
		canceledTaskRan.store(true);
	}, ve::tasks::BackgroundTaskOptions{ .cancellationId = meshBuildCancellationId }));

	CHECK(queue.CancelPendingTasks(meshBuildCancellationId) == 1U);
	CHECK(queue.Stats().canceledTaskCount == 1U);

	releaseCurrentTask.store(true);
	REQUIRE(WaitForBackgroundTaskQueueCondition([&queue]
	{
		return queue.Stats().completedTaskCount == 1U;
	}));
	CHECK(!canceledTaskRan.load());
}
