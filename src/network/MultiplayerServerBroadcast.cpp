#include "MultiplayerServer.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	std::size_t MultiplayerServer::Broadcast(const NetworkMessage& message)
	{
		return BroadcastExcept(0, message);
	}

	std::size_t MultiplayerServer::BroadcastExcept(std::uint32_t excludedConnectionId, const NetworkMessage& message)
	{
		ReapFinishedClientWorkers();
		std::size_t sent_message_count = 0;
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		for (const auto& clientWorker : _clientWorkers)
		{
			if (clientWorker->connectionId == excludedConnectionId) continue;
			if (clientWorker->socket &&
				!clientWorker->finished.load(std::memory_order_acquire))
			{
				NetworkMessage outboundMessage = message;
				outboundMessage.sequenceNumber = clientWorker->nextOutboundSequenceNumber++;
				if (SendNetworkMessage(
					*clientWorker->socket, outboundMessage, _lifecycleStopSource.get_token()))
				{
					++sent_message_count;
				}
				else clientWorker->socket->Shutdown();
			}
		}
		return sent_message_count;
	}
}
