#pragma once

#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSessionTypes.h"
#include "WorldEvent.h"

#include <span>
#include <unordered_map>
#include <vector>

namespace ve::world
{
	class World;
}

namespace ve::network
{
	/** Coordinates transport ownership, publication, and remote world mutations. */
	class NetworkSession
	{
	public:
		bool HostGame(const NetworkHostSettings& settings);
		bool JoinGame(const NetworkJoinSettings& settings);
		void Stop();
		std::size_t PublishWorldEvents(std::span<const ve::world::WorldEvent> worldEvents);
		NetworkPumpStats ApplyIncomingMessages(ve::world::World& world);
		[[nodiscard]] NetworkSessionMode Mode() const noexcept;
		[[nodiscard]] bool IsOnline() const noexcept;
		[[nodiscard]] NetworkSessionError LastError() const noexcept;
		std::vector<NetworkSessionEvent> DrainEvents();

	private:
		std::size_t PublishMessage(const NetworkMessage& message);
		void QueueLiveMutationMessages(std::vector<NetworkMessage> messages);
		std::size_t FlushOutboundMessages();
		NetworkPumpStats ApplyServerMessages(ve::world::World& world);
		NetworkPumpStats ApplyClientMessages(ve::world::World& world);
		void RecordSessionEvent(NetworkSessionEventType eventType, NetworkSessionError error);

		NetworkSessionMode _mode = NetworkSessionMode::Offline;
		NetworkSessionError _lastError = NetworkSessionError::None;
		NetworkHostSettings _activeHostSettings;
		MultiplayerClient _client;
		MultiplayerServer _server;
		NetworkSequenceTracker _serverToClientSequenceTracker;
		std::unordered_map<std::uint32_t, NetworkSequenceTracker> _clientSequenceTrackersByConnectionId;
		std::vector<NetworkMessage> _outboundWorldSnapshotMessages;
		std::vector<NetworkMessage> _outboundLiveMutationMessages;
		std::vector<NetworkSessionEvent> _pendingSessionEvents;
	};
}
