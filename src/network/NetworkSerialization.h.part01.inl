
#include "NetworkProtocol.h"

#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace ve::network
{
	inline constexpr std::uint16_t MaxPlayerNameByteCount = 32;

	/**
	 * Client handshake payload sent before gameplay messages.
	 */
	struct ClientHelloPayload
	{
		std::string playerName;
		std::uint32_t capabilityFlags = SupportedProtocolCapabilityFlags;
	};

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
	 * Serializes the first client identity and capability packet.
	 *
	 * @param clientHello Display name plus supported protocol capabilities.
	 * @return Payload bytes for a ClientHello message.
	 */
	ByteBuffer SerializeClientHello(const ClientHelloPayload& clientHello);

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

