#include "NetworkSession.h"

#include "Logger.h"
#include "NetworkBlockReplication.h"
#include "NetworkSerialization.h"

#include <iterator>
#include <optional>

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

	std::size_t NetworkSession::PublishWorldEvents(std::span<const ve::world::WorldEvent> worldEvents)
	{
		QueueLiveMutationMessages(BuildBlockMutationMessages(worldEvents));
		return FlushOutboundMessages();
	}

	void NetworkSession::QueueLiveMutationMessages(std::vector<NetworkMessage> messages)
	{
		_outboundLiveMutationMessages.insert(
			_outboundLiveMutationMessages.end(),
			std::make_move_iterator(messages.begin()),
			std::make_move_iterator(messages.end()));
	}

	std::size_t NetworkSession::FlushOutboundMessages()
	{
		std::size_t publishedMessages = 0;
		for (const NetworkMessage& message : _outboundWorldSnapshotMessages)
		{
			publishedMessages += PublishMessage(message);
		}
		for (const NetworkMessage& message : _outboundLiveMutationMessages)
		{
			publishedMessages += PublishMessage(message);
		}
		_outboundWorldSnapshotMessages.clear();
		_outboundLiveMutationMessages.clear();
		return publishedMessages;
	}

	std::size_t NetworkSession::PublishMessage(const NetworkMessage& message)
	{
		if (_mode == NetworkSessionMode::Hosting)
		{
			return _server.Broadcast(message);
		}

		if (_mode != NetworkSessionMode::Joined) return 0;
		const std::optional<BlockMutationPayload> mutation = TryDeserializeBlockMutation(message.payloadBytes);
		if (!mutation) return 0;
		return _client.SendBlockMutation(*mutation) ? 1 : 0;
	}

	NetworkSessionMode NetworkSession::Mode() const noexcept
	{
		return _mode;
	}

	bool NetworkSession::IsOnline() const noexcept
	{
		return _mode != NetworkSessionMode::Offline;
	}

	NetworkSessionError NetworkSession::LastError() const noexcept
	{
		return _lastError;
	}

	std::vector<NetworkSessionEvent> NetworkSession::DrainEvents()
	{
		std::vector<NetworkSessionEvent> drainedEvents;
		drainedEvents.swap(_pendingSessionEvents);
		return drainedEvents;
	}

	void NetworkSession::RecordSessionEvent(NetworkSessionEventType eventType, NetworkSessionError error)
	{
		_pendingSessionEvents.push_back(NetworkSessionEvent{ eventType, _mode, error });
	}
}
