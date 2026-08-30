#pragma once

#include "NetworkPayloadTypes.h"

#include <optional>
#include <span>

namespace ve::network
{
	/** @return Player name, or empty when the payload length is invalid. */
	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes);

	/** @return Client hello data, or empty when the payload is invalid. */
	std::optional<ClientHelloPayload> TryDeserializeClientHelloPayload(std::span<const std::byte> payloadBytes);

	/** @return Player snapshot data, or empty when the payload length is invalid. */
	std::optional<PlayerSnapshotPayload> TryDeserializePlayerSnapshot(std::span<const std::byte> payloadBytes);

	/** @return Block coordinate and target id, or empty when the payload length is invalid. */
	std::optional<BlockMutationPayload> TryDeserializeBlockMutation(std::span<const std::byte> payloadBytes);
}
