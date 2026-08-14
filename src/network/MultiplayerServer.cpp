#include "MultiplayerServer.h"

#include <utility>

namespace ve::network
{
	MultiplayerServer::~MultiplayerServer()
	{
		Stop();
	}

	bool MultiplayerServer::Start(const TcpListenSettings& listenSettings, std::size_t maxConnectedClients)
	{
		Stop();
		std::optional<TcpSocket> listeningSocket = TcpSocket::Listen(listenSettings);
		if (!listeningSocket) return false;
		_maxConnectedClients = maxConnectedClients == 0 ? 1 : maxConnectedClients;
		_listeningSocket = std::make_shared<TcpSocket>(std::move(*listeningSocket));
		_acceptThread = std::jthread([this](std::stop_token stopToken) { AcceptClientsUntilStopped(stopToken); });
		return true;
	}

	void MultiplayerServer::Stop()
	{
		if (_acceptThread.joinable()) _acceptThread.request_stop();
		if (_listeningSocket) _listeningSocket->Close();
		if (_acceptThread.joinable()) _acceptThread.join();
		_listeningSocket.reset();

		std::vector<std::jthread> clientThreads;
		{
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			for (std::jthread& clientThread : _clientThreads) clientThread.request_stop();
			for (ConnectedClient& connectedClient : _connectedClients)
			{
				if (connectedClient.socket) connectedClient.socket->Close();
			}
			_connectedClients.clear();
			clientThreads = std::move(_clientThreads);
		}
		clientThreads.clear();
		static_cast<void>(_incomingMessages.Drain());
	}

	std::vector<MultiplayerInboundMessage> MultiplayerServer::DrainIncomingMessages()
	{
		return _incomingMessages.Drain();
	}
}
