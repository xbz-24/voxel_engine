#include "BackgroundTaskQueue.h"

#include <algorithm>
#include <chrono>
#include <iterator>
#include <utility>

namespace ve::tasks
{
	/**
	 * Pushes one task for a worker thread.
	 *
	 * @param task Callable executed by a background worker.
	 * @return True when the task was accepted.
	 */
	bool BackgroundTaskQueue::Enqueue(BackgroundTask task)
	{
		return Enqueue(std::move(task), BackgroundTaskOptions{});
	}

	/**
	 * Pushes one task with scheduling metadata.
	 *
	 * @param task Callable executed by a background worker.
	 * @param options Priority and optional cancellation id for queued work.
	 * @return True when the task was accepted.
	 */
	bool BackgroundTaskQueue::Enqueue(BackgroundTask task, BackgroundTaskOptions options)
	{
		if (!task)
		{
			std::lock_guard<std::mutex> taskLock(_taskMutex);
			_stats.rejectedTaskCount++;
			return false;
		}

		{
			std::lock_guard<std::mutex> taskLock(_taskMutex);
			if (_isStopping)
			{
				_stats.rejectedTaskCount++;
				return false;
			}
			_tasks.push_back(QueuedBackgroundTask{
				std::move(task),
				std::chrono::steady_clock::now(),
				options,
				_nextEnqueueOrder++
			});
			_stats.acceptedTaskCount++;
		}
		_taskAvailable.notify_one();
		return true;
	}

	/**
	 * Removes queued tasks that have not started yet.
	 *
	 * @param cancellationId Non-zero cancellation id assigned at enqueue time.
	 * @return Number of pending tasks removed.
	 */
	std::size_t BackgroundTaskQueue::CancelPendingTasks(std::uint64_t cancellationId)
	{
		if (cancellationId == NoBackgroundTaskCancellationId) return 0;
		std::lock_guard<std::mutex> taskLock(_taskMutex);
		const auto firstTaskToCancel = std::remove_if(
			_tasks.begin(),
			_tasks.end(),
			[cancellationId](const QueuedBackgroundTask& queuedTask)
			{
				return queuedTask.options.cancellationId == cancellationId;
			});
		const std::size_t canceledTaskCount = static_cast<std::size_t>(std::distance(firstTaskToCancel, _tasks.end()));
		_tasks.erase(firstTaskToCancel, _tasks.end());
		_stats.canceledTaskCount += canceledTaskCount;
		return canceledTaskCount;
	}
}
