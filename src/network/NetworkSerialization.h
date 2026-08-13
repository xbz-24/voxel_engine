#pragma once

#include "NetworkDeserialization.h"

#include <string>

namespace ve::network
{
	/** @return Payload bytes for a ClientHello message. */
	ByteBuffer SerializeClientHello(const std::string& playerName);

	/** @return Payload bytes for a ClientHello message. */
	ByteBuffer SerializeClientHello(const ClientHelloPayload& clientHello);

	/** @return Payload bytes for a PlayerSnapshot message. */
	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot);

	/** @return Payload bytes for a BlockMutation message. */
	ByteBuffer SerializeBlockMutation(const BlockMutationPayload& blockMutation);
}
