#include "MultiplayerClient.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	bool MultiplayerClient::SendPlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot)
	{
		return SendMessage(NetworkMessage{ NetworkMessageType::PlayerSnapshot, SerializePlayerSnapshot(playerSnapshot) });
	}

	bool MultiplayerClient::SendBlockMutation(const BlockMutationPayload& blockMutation)
	{
		return SendMessage(NetworkMessage{ NetworkMessageType::BlockMutation, SerializeBlockMutation(blockMutation) });
	}

	bool MultiplayerClient::SendMessage(NetworkMessage message)
	{
		std::lock_guard<std::mutex> sendLock(_sendMutex);
		if (!_isConnected || !_connectedSocket) return false;
		message.sequenceNumber = _nextOutboundSequenceNumber.fetch_add(1, std::memory_order_relaxed);
		if (SendNetworkMessage(*_connectedSocket, message)) return true;
		_connectedSocket->Shutdown();
		_isConnected = false;
		return false;
	}
}
