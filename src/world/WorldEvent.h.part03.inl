		ChunkUnloadedEvent,
		EntityChangedEvent,
		RegionSavedEvent>;

	/**
	 * Single event record emitted by world APIs.
	 */
	struct WorldEvent
	{
		explicit WorldEvent(BlockChangedEvent block_changed_event) noexcept
			: payload(block_changed_event)
		{
		}

		explicit WorldEvent(ChunkGeneratedEvent chunk_generated_event) noexcept
			: payload(chunk_generated_event)
		{
		}

		explicit WorldEvent(ChunkLoadedEvent chunk_loaded_event) noexcept
			: payload(chunk_loaded_event)
		{
		}

		explicit WorldEvent(ChunkUnloadedEvent chunk_unloaded_event) noexcept
			: payload(chunk_unloaded_event)
		{
		}

		explicit WorldEvent(EntityChangedEvent entity_changed_event) noexcept
			: payload(std::move(entity_changed_event))
		{
		}

		explicit WorldEvent(RegionSavedEvent region_saved_event) noexcept
			: payload(std::move(region_saved_event))
		{
		}

		[[nodiscard]] WorldEventType Type() const noexcept
		{
			if (std::holds_alternative<BlockChangedEvent>(payload)) return WorldEventType::BlockChanged;
			if (std::holds_alternative<ChunkGeneratedEvent>(payload)) return WorldEventType::ChunkGenerated;
			if (std::holds_alternative<ChunkLoadedEvent>(payload)) return WorldEventType::ChunkLoaded;
			if (std::holds_alternative<ChunkUnloadedEvent>(payload)) return WorldEventType::ChunkUnloaded;
			if (std::holds_alternative<EntityChangedEvent>(payload)) return WorldEventType::EntityChanged;
			return WorldEventType::RegionSaved;
		}

		[[nodiscard]] const BlockChangedEvent* AsBlockChanged() const noexcept
		{
			return std::get_if<BlockChangedEvent>(&payload);
		}

		[[nodiscard]] const ChunkGeneratedEvent* AsChunkGenerated() const noexcept
		{
			return std::get_if<ChunkGeneratedEvent>(&payload);
		}

		[[nodiscard]] const ChunkLoadedEvent* AsChunkLoaded() const noexcept
		{
			return std::get_if<ChunkLoadedEvent>(&payload);
		}

		[[nodiscard]] const ChunkUnloadedEvent* AsChunkUnloaded() const noexcept
		{
			return std::get_if<ChunkUnloadedEvent>(&payload);
		}

		[[nodiscard]] const EntityChangedEvent* AsEntityChanged() const noexcept
		{
			return std::get_if<EntityChangedEvent>(&payload);
		}

		[[nodiscard]] const RegionSavedEvent* AsRegionSaved() const noexcept
		{
			return std::get_if<RegionSavedEvent>(&payload);
		}

		WorldEventPayload payload;
	};
}
