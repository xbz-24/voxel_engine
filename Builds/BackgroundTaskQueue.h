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
		 * Reports queued work that has not started yet.
		 *
		 * @return Number of pending tasks.
		 */
		std::size_t PendingTaskCount() const;

		/**
		 * Requests all workers to stop and waits for them.
		 */
		void Stop();

	private:
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
