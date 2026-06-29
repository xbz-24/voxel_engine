#include "MultiplayerServer.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	void MultiplayerServer::Broadcast(const NetworkMessage& message)
	{
		BroadcastExcept(0, message);
	}

	void MultiplayerServer::BroadcastExcept(std::uint32_t excludedConnectionId, const NetworkMessage& message)
	{
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		for (ConnectedClient& connectedClient : _connectedClients)
		{
			if (connectedClient.connectionId == excludedConnectionId) continue;
			if (connectedClient.socket && connectedClient.socket->IsOpen())
			{
				SendNetworkMessage(*connectedClient.socket, message);
			}
		}
	}
}
