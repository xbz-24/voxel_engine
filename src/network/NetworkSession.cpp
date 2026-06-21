#include "NetworkSession.h"

#include "Logger.h"
#include "NetworkBlockReplication.h"
#include "NetworkSerialization.h"

#include <optional>

namespace ve::network
{
	bool NetworkSession::HostGame(const NetworkHostSettings& settings)
	{
		Stop();
		const bool started = _server.Start(TcpListenSettings{ NetworkEndpoint{ "", settings.port }, settings.pendingConnectionBacklog });
		_mode = started ? NetworkSessionMode::Hosting : NetworkSessionMode::Offline;
		if (started) VE_LOG_CATEGORY_INFO(ve::log::category::Network, "Network host started");
		return started;
	}

	bool NetworkSession::JoinGame(const NetworkJoinSettings& settings)
	{
		Stop();
		const bool connected = _client.Connect(settings.serverEndpoint, settings.playerName);
		_mode = connected ? NetworkSessionMode::Joined : NetworkSessionMode::Offline;
		if (connected) VE_LOG_CATEGORY_INFO(ve::log::category::Network, "Network client connected");
		return connected;
	}

	void NetworkSession::Stop()
	{
		_client.Disconnect();
		_server.Stop();
		_mode = NetworkSessionMode::Offline;
	}

	std::size_t NetworkSession::PublishWorldEvents(std::span<const ve::world::WorldEvent> worldEvents)
	{
		std::size_t publishedMessages = 0;
		for (const NetworkMessage& message : BuildBlockMutationMessages(worldEvents))
		{
			publishedMessages += PublishMessage(message);
		}
		return publishedMessages;
	}

	std::size_t NetworkSession::PublishMessage(const NetworkMessage& message)
	{
		if (_mode == NetworkSessionMode::Hosting)
		{
			_server.Broadcast(message);
			return 1;
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
}
