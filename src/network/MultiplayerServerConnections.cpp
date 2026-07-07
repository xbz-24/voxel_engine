#include "MultiplayerServer.h"

#include "NetworkPacketIO.h"
#include "NetworkSerialization.h"

namespace ve::network
{
	namespace
	{
		bool IsValidClientHelloMessage(const NetworkMessage& message)
		{
			if (message.messageType != NetworkMessageType::ClientHello) return false;
			const std::optional<ClientHelloPayload> clientHello =
				TryDeserializeClientHelloPayload(message.payloadBytes);
			return clientHello.has_value();
		}
	}

	void MultiplayerServer::AcceptClientsUntilStopped(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested() && _listeningSocket && _listeningSocket->IsOpen())
		{
			std::optional<TcpSocket> acceptedSocket = _listeningSocket->Accept();
			if (!acceptedSocket) break;
			const std::uint32_t newConnectionId = _nextConnectionId.fetch_add(1);
			auto clientSocket = std::make_shared<TcpSocket>(std::move(*acceptedSocket));
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			std::size_t openClientConnectionCount = 0;
			for (const ConnectedClient& connectedClient : _connectedClients)
			{
				if (connectedClient.socket && connectedClient.socket->IsOpen()) ++openClientConnectionCount;
			}
			if (openClientConnectionCount >= _maxConnectedClients)
			{
				clientSocket->Close();
				continue;
			}
			_connectedClients.push_back(ConnectedClient{ newConnectionId, clientSocket, 1 });
			_clientThreads.emplace_back([this, newConnectionId, clientSocket](std::stop_token clientStopToken)
			{
				ReceiveClientMessages(clientStopToken, newConnectionId, clientSocket);
			});
		}
	}

	void MultiplayerServer::ReceiveClientMessages(std::stop_token stopToken, std::uint32_t connectionId, std::shared_ptr<TcpSocket> clientSocket)
	{
		if (stopToken.stop_requested() || !clientSocket || !clientSocket->IsOpen()) return;
		std::optional<NetworkMessage> clientHelloMessage = ReceiveNetworkMessage(*clientSocket);
		if (!clientHelloMessage || !IsValidClientHelloMessage(*clientHelloMessage))
		{
			if (clientSocket) clientSocket->Close();
			return;
		}

		while (!stopToken.stop_requested() && clientSocket && clientSocket->IsOpen())
		{
			std::optional<NetworkMessage> receivedMessage = ReceiveNetworkMessage(*clientSocket);
			if (!receivedMessage) break;
			_incomingMessages.Push(MultiplayerInboundMessage{ connectionId, std::move(*receivedMessage) });
		}
		if (clientSocket) clientSocket->Close();
	}
}
