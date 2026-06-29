namespace
{
	[[nodiscard]] int ToSerializedBlock(voxel::Block block) noexcept
	{
		return static_cast<int>(static_cast<std::uint8_t>(block));
	}

	[[nodiscard]] voxel::Block FromSerializedBlock(int value) noexcept
	{
		if (value < ToSerializedBlock(voxel::Block::Air) || value > ToSerializedBlock(voxel::Block::DeadBush))
		{
			return voxel::Block::Air;
		}
		return static_cast<voxel::Block>(static_cast<std::uint8_t>(value));
	}
}

namespace voxel
{
	BlockBox BlockBox::Between(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox{
			{
				std::min(first.x, second.x),
				std::min(first.y, second.y),
				std::min(first.z, second.z),
			},
			{
				std::max(first.x, second.x),
				std::max(first.y, second.y),
				std::max(first.z, second.z),
			}
		};
	}

	BlockBox Box(BlockPosition first, BlockPosition second) noexcept
	{
		return BlockBox::Between(first, second);
	}

	WorldConfig WorldConfig::SizeChunks(int value)
	{
		WorldConfig config{};
		config.size_chunks = value;
		return config;
	}

	WorldConfig& WorldConfig::WithSizeChunks(int value) noexcept
	{
		size_chunks = value;
		return *this;
	}

	WorldConfig World(int size_chunks)
	{
		return WorldConfig::SizeChunks(size_chunks);
	}

	WorldConfig World()
	{
		return {};
	}

	WorldConfig Scene(int size_chunks)
	{
		return World(size_chunks);
	}

	WorldConfig Scene()
	{
		return World();
	}

