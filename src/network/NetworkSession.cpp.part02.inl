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
