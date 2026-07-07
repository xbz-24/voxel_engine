#include "BackgroundTaskQueue.h"

#include <algorithm>
#include <utility>

namespace ve::tasks
{
	/**
	 * Creates worker threads.
	 *
	 * @param workerCount Number of worker loops to create.
	 */
	void BackgroundTaskQueue::StartWorkers(std::size_t workerCount)
	{
		_workers.reserve(workerCount);
		for (std::size_t workerIndex = 0; workerIndex < workerCount; workerIndex++)
		{
			_workers.emplace_back([this](std::stop_token stopToken) { RunWorker(stopToken); });
		}
	}

	/**
	 * Runs tasks until the queue is stopped.
	 *
	 * @param stopToken Cooperative stop signal owned by the worker thread.
	 */
	void BackgroundTaskQueue::RunWorker(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested())
		{
			QueuedBackgroundTask queuedTask;
			if (!WaitForTask(stopToken, queuedTask)) return;
			const std::chrono::steady_clock::time_point taskStartTime = std::chrono::steady_clock::now();
			RecordTaskStarted(taskStartTime - queuedTask.enqueuedAt);
			queuedTask.task();
			RecordTaskCompleted(std::chrono::steady_clock::now() - taskStartTime);
		}
	}

	/**
	 * Waits for one task and moves it out of the queue.
	 *
	 * @param stopToken Cooperative stop signal owned by the worker thread.
	 * @param queuedTask Destination filled when work is available.
	 * @return True when a task was loaded.
	 */
	bool BackgroundTaskQueue::WaitForTask(
		std::stop_token stopToken,
		BackgroundTaskQueue::QueuedBackgroundTask& queuedTask)
	{
		std::unique_lock<std::mutex> taskLock(_taskMutex);
		_taskAvailable.wait(taskLock, stopToken, [this] { return _isStopping || !_tasks.empty(); });
		if (_isStopping || _tasks.empty()) return false;
		const auto nextTask = std::min_element(
			_tasks.begin(),
			_tasks.end(),
			[](const QueuedBackgroundTask& leftTask, const QueuedBackgroundTask& rightTask)
			{
				if (leftTask.options.taskPriority != rightTask.options.taskPriority)
				{
					return leftTask.options.taskPriority > rightTask.options.taskPriority;
				}
				return leftTask.enqueueOrder < rightTask.enqueueOrder;
			});
		queuedTask = std::move(*nextTask);
		_tasks.erase(nextTask);
		return true;
	}

	void BackgroundTaskQueue::RecordTaskStarted(std::chrono::steady_clock::duration queueLatency)
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		_stats.startedTaskCount++;
		const std::chrono::nanoseconds measuredQueueLatency = std::chrono::duration_cast<std::chrono::nanoseconds>(queueLatency);
		_stats.longestQueueLatency = std::max(_stats.longestQueueLatency, measuredQueueLatency);
	}

	void BackgroundTaskQueue::RecordTaskCompleted(std::chrono::steady_clock::duration executionTime)
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		_stats.completedTaskCount++;
		const std::chrono::nanoseconds measuredExecutionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(executionTime);
		_stats.longestExecutionTime = std::max(_stats.longestExecutionTime, measuredExecutionTime);
	}

	void BackgroundTaskQueue::RecordShutdownWait(std::chrono::steady_clock::duration shutdownWait)
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		const std::chrono::nanoseconds measuredShutdownWait = std::chrono::duration_cast<std::chrono::nanoseconds>(shutdownWait);
		_stats.longestShutdownWait = std::max(_stats.longestShutdownWait, measuredShutdownWait);
	}
}
