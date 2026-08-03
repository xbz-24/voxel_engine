	inline constexpr BlockFaceOrdinal BlockFaceCount = ve::core::ToIndex(BlockFace::Count);
	inline constexpr std::size_t BlockFaceTextureSlotCount = BlockTypeCount * BlockFaceCount;
	inline constexpr std::array<BlockFace, BlockFaceCount> AllBlockFaces{ {
		BlockFace::Top,
		BlockFace::Bottom,
		BlockFace::Front,
		BlockFace::Back,
		BlockFace::Left,
		BlockFace::Right
	} };

	template <typename Value>
	using PerBlockFaceArray = std::array<Value, BlockFaceCount>;

	[[nodiscard]] constexpr BlockOrdinal ToBlockOrdinal(BlockId id) noexcept
	{
		return ve::core::ToIndex(id);
	}

	[[nodiscard]] constexpr BlockFaceOrdinal ToBlockFaceOrdinal(BlockFace face) noexcept
	{
		return ve::core::ToIndex(face);
	}

	[[nodiscard]] constexpr std::uint8_t ToBlockByte(BlockId id) noexcept
	{
		return ve::core::ToU8(id);
	}

	[[nodiscard]] constexpr BlockId BlockIdFromByte(std::uint8_t id) noexcept
	{
		return ve::core::NumericCast<BlockId>(id);
	}

	[[nodiscard]] constexpr bool IsStoredBlockId(BlockId id) noexcept
	{
		return ToBlockOrdinal(id) < BlockTypeCount;
	}

	[[nodiscard]] constexpr bool IsStoredBlockByte(std::uint8_t id) noexcept
	{
		return id < ToBlockByte(BlockId::Count);
	}

	[[nodiscard]] constexpr std::size_t BlockFaceTextureSlot(BlockId block, BlockFace face) noexcept
	{
		return (ToBlockOrdinal(block) * BlockFaceCount) + ToBlockFaceOrdinal(face);
	}
}
