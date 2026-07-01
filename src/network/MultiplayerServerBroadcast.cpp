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
		std::size_t sent_message_count = 0;
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		for (ConnectedClient& connectedClient : _connectedClients)
		{
			if (connectedClient.connectionId == excludedConnectionId) continue;
			if (connectedClient.socket && connectedClient.socket->IsOpen())
			{
				NetworkMessage outboundMessage = message;
				outboundMessage.sequenceNumber = connectedClient.nextOutboundSequenceNumber++;
				if (SendNetworkMessage(*connectedClient.socket, outboundMessage))
				{
					++sent_message_count;
				}
			}
		}
		return sent_message_count;
	}
}
