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
		if (!_connectedSocket || !_connectedSocket->IsOpen()) return false;
		return SendNetworkMessage(*_connectedSocket, message);
	}
}
