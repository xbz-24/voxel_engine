#include "WorkerPolicy.h"

#include <thread>

namespace ve::tasks
{
	/// Chooses a conservative background worker count for CPU engine jobs.
	ve::core::Index DefaultWorkerCount(ve::core::Index reservedThreadCount) noexcept
	{
		const unsigned int hardwareThreadCount = std::thread::hardware_concurrency();
		if (hardwareThreadCount <= reservedThreadCount + 1) return 1;
		return static_cast<ve::core::Index>(hardwareThreadCount) - reservedThreadCount;
	}
}
