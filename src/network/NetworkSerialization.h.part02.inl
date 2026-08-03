	/**
	 * Decodes a ClientHello payload.
	 *
	 * @param payloadBytes Payload bytes from a ClientHello message.
	 * @return Player name, or empty when the payload length is invalid.
	 */
	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes);

	/**
	 * Decodes a ClientHello payload with protocol capabilities.
	 *
	 * @param payloadBytes Payload bytes from a ClientHello message.
	 * @return Client hello data, or empty when the payload is invalid.
	 */
	std::optional<ClientHelloPayload> TryDeserializeClientHelloPayload(std::span<const std::byte> payloadBytes);

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
