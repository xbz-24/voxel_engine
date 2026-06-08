#include "MultiplayerServer.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	void MultiplayerServer::Broadcast(const NetworkMessage& message)
	{
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		for (ConnectedClient& connectedClient : _connectedClients)
		{
			if (connectedClient.socket && connectedClient.socket->IsOpen())
			{
				SendNetworkMessage(*connectedClient.socket, message);
			}
		}
	}
}
