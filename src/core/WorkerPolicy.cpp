#include "WorkerPolicy.h"

#include <thread>

namespace ve::tasks
{
	/// Chooses a conservative background worker count for CPU engine jobs.
	ve::core::Index DefaultWorkerCount(ve::core::Index reserved_threads) noexcept
	{
		const unsigned int hardware_threads = std::thread::hardware_concurrency();
		if (hardware_threads <= reserved_threads + 1) return 1;
		return static_cast<ve::core::Index>(hardware_threads) - reserved_threads;
	}
}
