#include "BackgroundTaskQueue.h"

#include <algorithm>
#include <exception>

namespace ve::tasks
{
	void BackgroundTaskQueue::RunWorker(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested())
		{
			QueuedBackgroundTask queuedTask;
			if (!WaitForTask(stopToken, queuedTask)) return;
			const auto taskStartTime = std::chrono::steady_clock::now();
			RecordTaskStarted(taskStartTime - queuedTask.enqueuedAt);
			std::exception_ptr failure;
			try
			{
				queuedTask.task();
			}
			catch (...)
			{
				failure = std::current_exception();
			}
			const auto taskEndTime = std::chrono::steady_clock::now();
			if (failure && queuedTask.options.failureHandler)
			{
				try { queuedTask.options.failureHandler(failure); }
				catch (...) {}
			}
			RecordTaskFinished(taskEndTime - taskStartTime, !failure);
		}
	}

	void BackgroundTaskQueue::RecordTaskFinished(
		std::chrono::steady_clock::duration executionTime,
		bool succeeded)
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		if (succeeded) _stats.completedTaskCount++;
		else _stats.failedTaskCount++;
		const auto measuredExecutionTime =
			std::chrono::duration_cast<std::chrono::nanoseconds>(executionTime);
		_stats.longestExecutionTime = std::max(_stats.longestExecutionTime, measuredExecutionTime);
	}
}
