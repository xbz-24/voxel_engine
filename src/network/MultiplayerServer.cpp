#include "MultiplayerServer.h"

namespace ve::network
{
	MultiplayerServer::~MultiplayerServer()
	{
		Stop();
	}

	bool MultiplayerServer::Start(const TcpListenSettings& listenSettings)
	{
		if (!_socketLibrary.IsAvailable()) return false;
		std::optional<TcpSocket> listeningSocket = TcpSocket::Listen(listenSettings);
		if (!listeningSocket) return false;
		_listeningSocket = std::make_shared<TcpSocket>(std::move(*listeningSocket));
		_acceptThread = std::jthread([this](std::stop_token stopToken) { AcceptClientsUntilStopped(stopToken); });
		return true;
	}

	void MultiplayerServer::Stop()
	{
		if (_acceptThread.joinable()) _acceptThread.request_stop();
		if (_listeningSocket) _listeningSocket->Close();
		std::lock_guard<std::mutex> clientsLock(_clientsMutex);
		for (ConnectedClient& connectedClient : _connectedClients)
		{
			if (connectedClient.socket) connectedClient.socket->Close();
		}
		_connectedClients.clear();
		_clientThreads.clear();
	}

	std::vector<MultiplayerInboundMessage> MultiplayerServer::DrainIncomingMessages()
	{
		return _incomingMessages.Drain();
	}
}
