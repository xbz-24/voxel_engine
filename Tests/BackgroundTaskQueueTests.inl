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
