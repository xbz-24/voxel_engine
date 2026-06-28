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
		// TODO: Add timestamp/tick id and velocity for interpolation and reconciliation.
		std::uint32_t playerId;
		float positionX, positionY, positionZ;
		float yawDegrees, pitchDegrees;
	};

	/**
	 * Block edit replicated between peers.
	 */
	struct BlockMutationPayload
	{
		// TODO: Add mutation id and author id so clients can de-duplicate predicted edits.
		std::int32_t blockX, blockY, blockZ;
		std::uint8_t blockId;
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
	 * @param playerSnapshot Player id, position, yaw, and pitch.
	 * @return Payload bytes for a PlayerSnapshot message.
	 */
	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot);

	/**
	 * Serializes one block mutation.
	 *
	 * @param blockMutation Block coordinate and target block id.
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
