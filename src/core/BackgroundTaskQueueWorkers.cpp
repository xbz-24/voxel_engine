#include "BackgroundTaskQueue.h"

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
			BackgroundTask task;
			if (!WaitForTask(stopToken, task)) return;
			task();
		}
	}

	/**
	 * Waits for one task and moves it out of the queue.
	 *
	 * @param stopToken Cooperative stop signal owned by the worker thread.
	 * @param task Destination callable filled when work is available.
	 * @return True when a task was loaded.
	 */
	bool BackgroundTaskQueue::WaitForTask(std::stop_token stopToken, BackgroundTask& task)
	{
		std::unique_lock<std::mutex> taskLock(_taskMutex);
		_taskAvailable.wait(taskLock, stopToken, [this] { return _isStopping || !_tasks.empty(); });
		if (_tasks.empty()) return false;
		task = std::move(_tasks.front());
		_tasks.pop();
		return true;
	}
}
