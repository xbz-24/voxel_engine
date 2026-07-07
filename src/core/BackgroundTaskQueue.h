#pragma once

#include "BackgroundTaskQueueTypes.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace ve::tasks
{
	class BackgroundTaskQueue
	{
	public:
		/**
		 * Starts worker threads for non-OpenGL engine work.
		 *
		 * @param workerCount Number of background workers to start.
		 */
		explicit BackgroundTaskQueue(std::size_t workerCount);

		/**
		 * Stops workers and joins their threads.
		 */
		~BackgroundTaskQueue();

		BackgroundTaskQueue(const BackgroundTaskQueue&) = delete;
		BackgroundTaskQueue& operator=(const BackgroundTaskQueue&) = delete;
		BackgroundTaskQueue(BackgroundTaskQueue&&) = delete;
		BackgroundTaskQueue& operator=(BackgroundTaskQueue&&) = delete;

		/**
		 * Pushes one task for a worker thread.
		 *
		 * @param task Callable executed by a background worker.
		 * @return True when the task was accepted.
		 */
		bool Enqueue(BackgroundTask task);

		/**
		 * Pushes one task with scheduling metadata.
		 *
		 * @param task Callable executed by a background worker.
		 * @param options Priority and optional cancellation id for queued work.
		 * @return True when the task was accepted.
		 */
		bool Enqueue(BackgroundTask task, BackgroundTaskOptions options);

		/**
		 * Removes queued tasks that have not started yet.
		 *
		 * @param cancellationId Non-zero cancellation id assigned at enqueue time.
		 * @return Number of pending tasks removed.
		 */
		std::size_t CancelPendingTasks(std::uint64_t cancellationId);

		/** @return Number of tasks still in the queue. */
		std::size_t PendingTaskCount() const;

		/** @return Snapshot of queue counters and timing telemetry. */
		BackgroundTaskQueueStats Stats() const;

		/** Signals all workers to stop and joins their threads. */
		void Stop();

	private:
		struct QueuedBackgroundTask
		{
			BackgroundTask task;
			std::chrono::steady_clock::time_point enqueuedAt;
			BackgroundTaskOptions options;
			std::uint64_t enqueueOrder = 0;
		};

		void StartWorkers(std::size_t workerCount);
		void RunWorker(std::stop_token stopToken);
		bool WaitForTask(std::stop_token stopToken, QueuedBackgroundTask& queuedTask);
		void RecordTaskStarted(std::chrono::steady_clock::duration queueLatency);
		void RecordTaskCompleted(std::chrono::steady_clock::duration executionTime);
		void RecordShutdownWait(std::chrono::steady_clock::duration shutdownWait);

		std::vector<std::jthread> _workers;
		std::vector<QueuedBackgroundTask> _tasks;
		mutable std::mutex _taskMutex;
		std::condition_variable_any _taskAvailable;
		BackgroundTaskQueueStats _stats;
		std::uint64_t _nextEnqueueOrder = 0;
		bool _isStopping = false;
	};
}
