#include "BackgroundTaskQueue.h"

#include <utility>

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
	 * Pushes one task for a worker thread.
	 *
	 * @param task Callable executed by a background worker.
	 * @return True when the task was accepted.
	 */
	bool BackgroundTaskQueue::Enqueue(BackgroundTask task)
	{
		{
			std::lock_guard<std::mutex> taskLock(_taskMutex);
			if (_isStopping) return false;
			_tasks.push(std::move(task));
		}
		_taskAvailable.notify_one();
		return true;
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
	 * Requests all workers to stop and waits for them.
	 */
	void BackgroundTaskQueue::Stop()
	{
		{
			std::lock_guard<std::mutex> taskLock(_taskMutex);
			_isStopping = true;
		}
		for (std::jthread& worker : _workers) worker.request_stop();
		_taskAvailable.notify_all();
		_workers.clear();
	}

}
