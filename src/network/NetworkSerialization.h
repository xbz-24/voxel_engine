#pragma once

#include "NetworkTypes.h"

#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace ve::network
{
	inline constexpr std::uint16_t MaxPlayerNameByteCount = 32;

	/**
	 * Player transform replicated from client to server.
	 */
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

	/**
	 * Block edit replicated between peers.
	 */
	struct BlockMutationPayload
	{
		std::uint32_t mutationId = 0;
		std::uint32_t authorPlayerId = 0;
		std::int32_t blockX = 0;
		std::int32_t blockY = 0;
		std::int32_t blockZ = 0;
		std::uint8_t blockId = 0;
	};

	/**
	 * Serializes the first client identity packet.
	 *
	 * @param playerName Display name sent to the server.
	 * @return Payload bytes for a ClientHello message.
	 */
	ByteBuffer SerializeClientHello(const std::string& playerName);

	/**
	 * Serializes one player transform update.
	 *
	 * @param playerSnapshot Player id, tick, position, velocity, yaw, and pitch.
	 * @return Payload bytes for a PlayerSnapshot message.
	 */
	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot);

	/**
	 * Serializes one block mutation.
	 *
	 * @param blockMutation Mutation metadata plus block coordinate and target block id.
	 * @return Payload bytes for a BlockMutation message.
	 */
	ByteBuffer SerializeBlockMutation(const BlockMutationPayload& blockMutation);

	/**
	 * Decodes a ClientHello payload.
	 *
	 * @param payloadBytes Payload bytes from a ClientHello message.
	 * @return Player name, or empty when the payload length is invalid.
	 */
	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes);

	/**
	 * Decodes a PlayerSnapshot payload.
	 *
	 * @param payloadBytes Payload bytes from a PlayerSnapshot message.
	 * @return Player snapshot data, or empty when the payload length is invalid.
	 */
	std::optional<PlayerSnapshotPayload> TryDeserializePlayerSnapshot(std::span<const std::byte> payloadBytes);

	/**
	 * Decodes a BlockMutation payload.
	 *
	 * @param payloadBytes Payload bytes from a BlockMutation message.
	 * @return Block coordinate and target id, or empty when the payload length is invalid.
	 */
	std::optional<BlockMutationPayload> TryDeserializeBlockMutation(std::span<const std::byte> payloadBytes);
}
