#include "NetworkSession.h"

#include "Logger.h"

namespace ve::network
{
	bool NetworkSession::HostGame(const NetworkHostSettings& settings)
	{
		Stop();
		if (settings.simulationTickRateHz == 0)
		{
			_lastError = NetworkSessionError::InvalidHostTickRate;
			RecordSessionEvent(NetworkSessionEventType::HostStartFailed, _lastError);
			return false;
		}

		const bool started = _server.Start(
			TcpListenSettings{ NetworkEndpoint{ "", settings.port }, settings.pendingConnectionBacklog },
			settings.maxConnectedClients);
		_mode = started ? NetworkSessionMode::Hosting : NetworkSessionMode::Offline;
		if (started)
		{
			_activeHostSettings = settings;
			_lastError = NetworkSessionError::None;
			RecordSessionEvent(NetworkSessionEventType::HostingStarted, NetworkSessionError::None);
			VE_LOG_CATEGORY_INFO(ve::log::category::Network, "Network host started");
		}
		else
		{
			_lastError = NetworkSessionError::HostStartFailed;
			RecordSessionEvent(NetworkSessionEventType::HostStartFailed, _lastError);
		}
		return started;
	}

	bool NetworkSession::JoinGame(const NetworkJoinSettings& settings)
	{
		Stop();
		const bool connected = _client.Connect(settings.serverEndpoint, settings.playerName);
		_mode = connected ? NetworkSessionMode::Joined : NetworkSessionMode::Offline;
		if (connected)
		{
			_lastError = NetworkSessionError::None;
			RecordSessionEvent(NetworkSessionEventType::Joined, NetworkSessionError::None);
			VE_LOG_CATEGORY_INFO(ve::log::category::Network, "Network client connected");
		}
		else
		{
			_lastError = NetworkSessionError::JoinFailed;
			RecordSessionEvent(NetworkSessionEventType::JoinFailed, _lastError);
		}
		return connected;
	}

	void NetworkSession::Stop()
	{
		const NetworkSessionMode previousMode = _mode;
		_client.Disconnect();
		_server.Stop();
		_serverToClientSequenceTracker.Reset();
		_clientSequenceTrackersByConnectionId.clear();
		_outboundWorldSnapshotMessages.clear();
		_outboundLiveMutationMessages.clear();
		_mode = NetworkSessionMode::Offline;
		_lastError = NetworkSessionError::None;
		if (previousMode != NetworkSessionMode::Offline)
		{
			RecordSessionEvent(NetworkSessionEventType::Stopped, NetworkSessionError::None);
		}
	}
}
