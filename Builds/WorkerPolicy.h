#pragma once

#include "CoreTypes.h"

namespace ve::tasks
{
	/**
	 * Chooses a conservative background worker count for CPU engine jobs.
	 *
	 * @param reserved_threads Threads kept free for render/game/OS work.
	 * @return Worker count clamped to at least one thread.
	 */
	ve::core::Index DefaultWorkerCount(ve::core::Index reserved_threads = 2) noexcept;
}
