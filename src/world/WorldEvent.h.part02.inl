		{
			return WorldEventFilter{
				.include_block_changed = false,
				.include_chunk_generated = false,
				.include_chunk_loaded = false,
				.include_chunk_unloaded = false,
				.include_entity_changed = false,
				.include_region_saved = true
			};
		}

		[[nodiscard]] constexpr bool Includes(WorldEventType type) const noexcept
		{
			switch (type)
			{
			case WorldEventType::BlockChanged: return include_block_changed;
			case WorldEventType::ChunkGenerated: return include_chunk_generated;
			case WorldEventType::ChunkLoaded: return include_chunk_loaded;
			case WorldEventType::ChunkUnloaded: return include_chunk_unloaded;
			case WorldEventType::EntityChanged: return include_entity_changed;
			case WorldEventType::RegionSaved: return include_region_saved;
			default: return false;
			}
		}
	};

	/**
	 * Describes one block mutation accepted by the world.
	 */
	struct BlockChangedEvent
	{
		glm::ivec3 worldBlockPosition;
		ve::blocks::BlockId previousBlockId;
		ve::blocks::BlockId newBlockId;
	};

	/**
	 * Describes one generated chunk entering the world.
	 */
	struct ChunkGeneratedEvent
	{
		int chunkCoordinateX;
		int chunkCoordinateZ;
	};

	/**
	 * Describes one chunk becoming available to the world.
	 */
	struct ChunkLoadedEvent
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
	};

	/**
	 * Describes one chunk leaving active world storage.
	 */
	struct ChunkUnloadedEvent
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
	};

	/**
	 * Describes a runtime or authored entity mutation observed by tooling.
	 */
	struct EntityChangedEvent
	{
		std::uint64_t entityId = 0;
		std::string reason;
	};

	/**
	 * Describes a saved chunk-region range.
	 */
	struct RegionSavedEvent
	{
		int minimumChunkCoordinateX = 0;
		int minimumChunkCoordinateZ = 0;
		int maximumChunkCoordinateX = 0;
		int maximumChunkCoordinateZ = 0;
		std::string destination;
	};

	using WorldEventPayload = std::variant<
		BlockChangedEvent,
		ChunkGeneratedEvent,
		ChunkLoadedEvent,
