#pragma once

#include "NetworkProtocol.h"

#include <cstdint>
#include <string>

namespace ve::network
{
	inline constexpr std::uint16_t MaxPlayerNameByteCount = 32;

	/** Client handshake payload sent before gameplay messages. */
	struct ClientHelloPayload
	{
		std::string playerName;
		std::uint32_t capabilityFlags = SupportedProtocolCapabilityFlags;
	};

	/** Player transform replicated from client to server. */
	struct PlayerSnapshotPayload
	{
		std::uint32_t playerId = 0;
		std::uint32_t simulationTickId = 0;
		float positionX = 0.0f;
		float positionY = 0.0f;
		float positionZ = 0.0f;
		float velocityX = 0.0f;
		float velocityY = 0.0f;
		float velocityZ = 0.0f;
		float yawDegrees = 0.0f;
		float pitchDegrees = 0.0f;
	};

	/** Block edit replicated between peers. */
	struct BlockMutationPayload
	{
		std::uint32_t mutationId = 0;
		std::uint32_t authorPlayerId = 0;
		std::int32_t blockX = 0;
		std::int32_t blockY = 0;
		std::int32_t blockZ = 0;
		std::uint8_t blockId = 0;
	};
}
