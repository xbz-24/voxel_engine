#include "MultiplayerServer.h"

namespace ve::network
{
	MultiplayerServerStats MultiplayerServer::Stats() const
	{
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		MultiplayerServerStats stats;
		stats.retainedClientWorkerCount = _clientWorkers.size();
		for (const auto& clientWorker : _clientWorkers)
		{
			if (clientWorker &&
				!clientWorker->finished.load(std::memory_order_acquire))
			{
				++stats.activeClientCount;
			}
		}
		return stats;
	}
}
