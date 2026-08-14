#include "MultiplayerServer.h"

#include <algorithm>

namespace ve::network
{
	void MultiplayerServer::ReapFinishedClientWorkers()
	{
		std::vector<std::unique_ptr<MultiplayerServerClientWorker>> finishedWorkers;
		{
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			finishedWorkers.reserve(_clientWorkers.size());
			for (auto& clientWorker : _clientWorkers)
			{
				if (clientWorker &&
					clientWorker->finished.load(std::memory_order_acquire))
				{
					finishedWorkers.push_back(std::move(clientWorker));
				}
			}
			std::erase(_clientWorkers, nullptr);
		}
	}
}
