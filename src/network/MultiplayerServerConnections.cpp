#include "MultiplayerServer.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	void MultiplayerServer::AcceptClientsUntilStopped(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested() && _listeningSocket && _listeningSocket->IsOpen())
		{
			std::optional<TcpSocket> acceptedSocket = _listeningSocket->Accept();
			if (!acceptedSocket) break;
			const std::uint32_t newConnectionId = _nextConnectionId.fetch_add(1);
			auto clientSocket = std::make_shared<TcpSocket>(std::move(*acceptedSocket));
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			_connectedClients.push_back(ConnectedClient{ newConnectionId, clientSocket });
			_clientThreads.emplace_back([this, newConnectionId, clientSocket](std::stop_token clientStopToken)
			{
				ReceiveClientMessages(clientStopToken, newConnectionId, clientSocket);
			});
		}
	}

	void MultiplayerServer::ReceiveClientMessages(std::stop_token stopToken, std::uint32_t connectionId, std::shared_ptr<TcpSocket> clientSocket)
	{
		while (!stopToken.stop_requested() && clientSocket && clientSocket->IsOpen())
		{
			std::optional<NetworkMessage> receivedMessage = ReceiveNetworkMessage(*clientSocket);
			if (!receivedMessage) break;
			_incomingMessages.Push(MultiplayerInboundMessage{ connectionId, std::move(*receivedMessage) });
		}
		if (clientSocket) clientSocket->Close();
	}
}
