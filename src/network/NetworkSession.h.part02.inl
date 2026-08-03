
	/** Queued lifecycle event reported by NetworkSession::DrainEvents. */
	struct NetworkSessionEvent
	{
		NetworkSessionEventType eventType = NetworkSessionEventType::Stopped;
		NetworkSessionMode mode = NetworkSessionMode::Offline;
		NetworkSessionError error = NetworkSessionError::None;
	};

	/** Coordinates client/server ownership, message publication, and incoming world mutation application. */
	class NetworkSession
	{
	public:
		/** @param settings Listen port and pending connection backlog. @return True when hosting started. */
		bool HostGame(const NetworkHostSettings& settings);
		/** @param settings Server endpoint plus local player name. @return True when the client connected. */
		bool JoinGame(const NetworkJoinSettings& settings);
		/** Stops client/server workers and returns to offline mode. */
		void Stop();
		/** @param worldEvents Events drained from the world. @return Number of outbound message writes accepted. */
		std::size_t PublishWorldEvents(std::span<const ve::world::WorldEvent> worldEvents);
		/** @param world World receiving remote mutations. @return Receive/apply counters for this pump. */
		NetworkPumpStats ApplyIncomingMessages(ve::world::World& world);
		/** @return Current high-level network mode. */
		NetworkSessionMode Mode() const noexcept;
		/** @return True when hosting or connected as a client. */
		bool IsOnline() const noexcept;
		/** @return Last session-level connection/startup error. */
		NetworkSessionError LastError() const noexcept;
		/** @return Session lifecycle events emitted since the previous drain. */
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
