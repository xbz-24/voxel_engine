#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>

namespace ve::tasks
{
	using BackgroundTask = std::function<void()>;
	using BackgroundTaskFailureHandler = std::function<void(std::exception_ptr)>;
	inline constexpr std::uint64_t NoBackgroundTaskCancellationId = 0;

	struct BackgroundTaskOptions
	{
		int taskPriority = 0;
		std::uint64_t cancellationId = NoBackgroundTaskCancellationId;
		/** Runs on the worker after a task throws; handler exceptions are contained. */
		BackgroundTaskFailureHandler failureHandler;
	};

	struct BackgroundTaskQueueStats
	{
		std::size_t pendingTaskCount = 0;
		std::size_t acceptedTaskCount = 0;
		std::size_t rejectedTaskCount = 0;
		std::size_t canceledTaskCount = 0;
		std::size_t startedTaskCount = 0;
		std::size_t completedTaskCount = 0;
		std::size_t failedTaskCount = 0;
		std::chrono::nanoseconds longestQueueLatency = std::chrono::nanoseconds{ 0 };
		std::chrono::nanoseconds longestExecutionTime = std::chrono::nanoseconds{ 0 };
		std::chrono::nanoseconds longestShutdownWait = std::chrono::nanoseconds{ 0 };
	};
}
