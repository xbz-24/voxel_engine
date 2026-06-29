#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ve::tasks
{
	using BackgroundTask = std::function<void()>;
	inline constexpr std::uint64_t NoBackgroundTaskCancellationId = 0;

	struct BackgroundTaskOptions
	{
		int taskPriority = 0;
		std::uint64_t cancellationId = NoBackgroundTaskCancellationId;
	};

	struct BackgroundTaskQueueStats
	{
		std::size_t pendingTaskCount = 0;
		std::size_t acceptedTaskCount = 0;
		std::size_t rejectedTaskCount = 0;
		std::size_t canceledTaskCount = 0;
		std::size_t startedTaskCount = 0;
		std::size_t completedTaskCount = 0;
		std::chrono::nanoseconds longestQueueLatency = std::chrono::nanoseconds{ 0 };
		std::chrono::nanoseconds longestExecutionTime = std::chrono::nanoseconds{ 0 };
		std::chrono::nanoseconds longestShutdownWait = std::chrono::nanoseconds{ 0 };
	};
}
