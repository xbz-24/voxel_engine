#include "BackgroundTaskQueue.h"

#include <chrono>

namespace ve::tasks
{
	/**
	 * Starts worker threads for non-OpenGL engine work.
	 *
	 * @param workerCount Number of background workers to start.
	 */
	BackgroundTaskQueue::BackgroundTaskQueue(std::size_t workerCount)
	{
		StartWorkers(workerCount);
	}

	/**
	 * Stops workers and joins their threads.
	 */
	BackgroundTaskQueue::~BackgroundTaskQueue()
	{
		Stop();
	}

	/**
	 * Reports queued work that has not started yet.
	 *
	 * @return Number of pending tasks.
	 */
	std::size_t BackgroundTaskQueue::PendingTaskCount() const
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		return _tasks.size();
	}

	/**
	 * Copies queue counters and timing telemetry.
	 *
	 * @return Current queue stats snapshot.
	 */
	BackgroundTaskQueueStats BackgroundTaskQueue::Stats() const
	{
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		BackgroundTaskQueueStats stats = _stats;
		stats.pendingTaskCount = _tasks.size();
		return stats;
	}

	/**
	 * Requests all workers to stop and waits for them.
	 */
	void BackgroundTaskQueue::Stop()
	{
		const std::chrono::steady_clock::time_point shutdownStartTime = std::chrono::steady_clock::now();
		{
			std::lock_guard<std::mutex> taskLock(_taskMutex);
			_isStopping = true;
		}
		for (std::jthread& worker : _workers) worker.request_stop();
		_taskAvailable.notify_all();
		_workers.clear();
		RecordShutdownWait(std::chrono::steady_clock::now() - shutdownStartTime);
	}

}
