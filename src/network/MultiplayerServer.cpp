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
		_lifecycleStopSource = std::stop_source{};
		_listeningSocket = std::make_shared<TcpSocket>(std::move(*listeningSocket));
		try
		{
			_acceptThread = std::jthread([this](std::stop_token stopToken)
			{
				try { AcceptClientsUntilStopped(stopToken); }
				catch (...) {}
			});
		}
		catch (...)
		{
			_listeningSocket->Close();
			_listeningSocket.reset();
			return false;
		}
		return true;
	}

	void MultiplayerServer::Stop()
	{
		_lifecycleStopSource.request_stop();
		if (_acceptThread.joinable()) _acceptThread.request_stop();
		if (_acceptThread.joinable()) _acceptThread.join();
		if (_listeningSocket) _listeningSocket->Close();
		_listeningSocket.reset();

		std::vector<std::unique_ptr<MultiplayerServerClientWorker>> clientWorkers;
		{
			std::lock_guard<std::mutex> clientsLock(_clientsMutex);
			for (const auto& clientWorker : _clientWorkers)
			{
				clientWorker->thread.request_stop();
				if (clientWorker->socket) clientWorker->socket->Shutdown();
			}
			clientWorkers = std::move(_clientWorkers);
		}
		clientWorkers.clear();
		static_cast<void>(_incomingMessages.Drain());
	}

	std::vector<MultiplayerInboundMessage> MultiplayerServer::DrainIncomingMessages()
	{
		return _incomingMessages.Drain();
	}
}
