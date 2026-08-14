#include "MultiplayerServer.h"

#include "MultiplayerInboxLimits.h"
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
		while (!stopToken.stop_requested() && _listeningSocket)
		{
			std::optional<TcpSocket> acceptedSocket = _listeningSocket->Accept(stopToken);
			if (!acceptedSocket) break;
			ReapFinishedClientWorkers();
			const std::uint32_t newConnectionId = _nextConnectionId.fetch_add(1);
			auto clientSocket = std::make_shared<TcpSocket>(std::move(*acceptedSocket));
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			if (_clientWorkers.size() >= _maxConnectedClients)
			{
				clientSocket->Close();
				continue;
			}
			auto clientWorker = std::make_unique<MultiplayerServerClientWorker>();
			clientWorker->connectionId = newConnectionId;
			clientWorker->socket = std::move(clientSocket);
			MultiplayerServerClientWorker* worker = clientWorker.get();
			_clientWorkers.push_back(std::move(clientWorker));
			try
			{
				worker->thread = std::jthread([this, worker](std::stop_token clientStopToken)
				{
					ReceiveClientMessages(clientStopToken, *worker);
					worker->finished.store(true, std::memory_order_release);
				});
			}
			catch (...)
			{
				worker->socket->Close();
				_clientWorkers.pop_back();
			}
		}
	}

	void MultiplayerServer::ReceiveClientMessages(
		std::stop_token stopToken, MultiplayerServerClientWorker& clientWorker)
	{
		try
		{
			if (!stopToken.stop_requested() && clientWorker.socket)
			{
				std::optional<NetworkMessage> hello = ReceiveNetworkMessage(*clientWorker.socket);
				if (hello && IsValidClientHelloMessage(*hello))
				{
					while (!stopToken.stop_requested())
					{
						auto message = ReceiveNetworkMessage(*clientWorker.socket);
						if (!message) break;
						if (!_incomingMessages.TryPush(
							{ clientWorker.connectionId, std::move(*message) },
							MultiplayerInboxMessageCapacity)) break;
					}
				}
			}
		}
		catch (...) {}
		if (clientWorker.socket) clientWorker.socket->Shutdown();
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		if (clientWorker.socket) clientWorker.socket->Close();
	}
}
