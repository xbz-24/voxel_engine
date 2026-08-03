			}
		}
		return accepted_definition_count;
	}

	bool BlockRegistry::Contains(BlockId id) const
	{
		return _blocks.find(id) != _blocks.end();
	}

	std::size_t BlockRegistry::RegisteredBlockCount() const noexcept
	{
		return _blocks.size();
	}

	const BlockType& BlockRegistry::Get(BlockId id) const
	{
		const auto block_iterator = _blocks.find(id);
		return block_iterator != _blocks.end() ? block_iterator->second : FallbackBlock();
	}

	bool BlockRegistry::IsAir(BlockId id) const
	{
		return id == BlockId::Air;
	}

	bool BlockRegistry::IsSolid(BlockId id) const
	{
		return Get(id).isSolid;
	}

	ve::rendering::TextureHandle BlockRegistry::TextureFor(BlockId id, BlockFace face) const
	{
		return Get(id).faceTextures[ToBlockFaceOrdinal(face)];
	}

	const ve::rendering::PbrMaterial& BlockRegistry::MaterialFor(BlockId id) const
	{
		return Get(id).material;
	}

	const BlockGameplayProperties& BlockRegistry::GameplayFor(BlockId id) const
	{
		return Get(id).gameplay;
	}

	bool BlockRegistry::BlocksMovement(BlockId id) const
	{
		return GameplayFor(id).collision == BlockCollisionMode::Solid;
	}

	bool BlockRegistry::IsTransparent(BlockId id) const
	{
		return GameplayFor(id).transparent;
	}

	bool BlockRegistry::IsRenderable(BlockId id) const
	{
		return id != BlockId::Air && Contains(id);
	}

	bool BlockRegistry::OccludesNeighborFaces(BlockId id) const
	{
		return IsRenderable(id) && !IsTransparent(id);
	}

	const BlockType& BlockRegistry::FallbackBlock() const noexcept
	{
		const auto air_iterator = _blocks.find(BlockId::Air);
		if (air_iterator != _blocks.end())
		{
			return air_iterator->second;
		}

		static const BlockType fallback_air_block{
			BlockId::Air,
			"Air",
			false,
			EmptyFaces(),
			ve::rendering::PbrMaterial{},
			BlockGameplayProperties{}
		};
		return fallback_air_block;
	}
}
