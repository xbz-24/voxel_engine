#pragma once

#include <cstddef>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace ve::tasks
{
	using BackgroundTask = std::function<void()>;

	// TODO: Add task priorities and cancellation ids so chunk meshing can drop work that moved out of view.
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

		/** @return Number of tasks still in the queue. */
		std::size_t PendingTaskCount() const;

		/** Signals all workers to stop and joins their threads. */
		void Stop();

	private:
		// TODO: Add worker telemetry for queue latency, execution time, and shutdown stalls.
		void StartWorkers(std::size_t workerCount);
		void RunWorker(std::stop_token stopToken);
		bool WaitForTask(std::stop_token stopToken, BackgroundTask& task);

		std::vector<std::jthread> _workers;
		std::queue<BackgroundTask> _tasks;
		mutable std::mutex _taskMutex;
		std::condition_variable_any _taskAvailable;
		bool _isStopping = false;
	};
}
