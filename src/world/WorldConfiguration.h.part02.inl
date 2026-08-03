	/**
	 * Calculates chunk storage needed by a square world.
	 *
	 * @param worldSizeChunks Number of chunks on one side of the square world.
	 * @return Total chunks needed to hold the square world.
	 */
	std::size_t ChunkCapacityForSquareWorld(int worldSizeChunks) noexcept;

	/**
	 * Builds creation settings sized for a square world.
	 *
	 * @param worldSizeChunks Number of chunks on one side of the square world.
	 * @return Creation info with enough chunk capacity for that world.
	 */
	WorldCreateInfo CreateInfoForSquareWorld(int worldSizeChunks) noexcept;

	/**
	 * Builds creation settings for chunk storage that grows as chunks stream in.
	 *
	 * @return Creation info that avoids reserving an entire world grid up front.
	 */
	WorldCreateInfo CreateInfoForStreamingWorld() noexcept;
}
