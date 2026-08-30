#include "NetworkSession.h"

#include "NetworkBlockReplication.h"
#include "NetworkSerialization.h"

#include <iterator>
#include <optional>

namespace ve::network
{
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
		if (_mode == NetworkSessionMode::Hosting) return _server.Broadcast(message);
		if (_mode != NetworkSessionMode::Joined) return 0;
		const std::optional<BlockMutationPayload> mutation = TryDeserializeBlockMutation(message.payloadBytes);
		if (!mutation) return 0;
		return _client.SendBlockMutation(*mutation) ? 1 : 0;
	}
}
