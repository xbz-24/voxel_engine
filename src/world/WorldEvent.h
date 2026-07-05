#pragma once

#include "Block.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <variant>

namespace ve::world
{
	/**
	 * Event categories emitted by the world simulation.
	 */
	enum class WorldEventType
	{
		BlockChanged,
		ChunkGenerated,
		ChunkLoaded,
		ChunkUnloaded,
		EntityChanged,
		RegionSaved
	};

	struct WorldEventFilter
	{
		bool include_block_changed = true;
		bool include_chunk_generated = true;
		bool include_chunk_loaded = true;
		bool include_chunk_unloaded = true;
		bool include_entity_changed = true;
		bool include_region_saved = true;

		[[nodiscard]] static constexpr WorldEventFilter All() noexcept
		{
			return {};
		}

		[[nodiscard]] static constexpr WorldEventFilter BlockChangesOnly() noexcept
		{
			return WorldEventFilter{
				.include_block_changed = true,
				.include_chunk_generated = false,
				.include_chunk_loaded = false,
				.include_chunk_unloaded = false,
				.include_entity_changed = false,
				.include_region_saved = false
			};
		}

		[[nodiscard]] static constexpr WorldEventFilter ChunkGenerationOnly() noexcept
		{
			return WorldEventFilter{
				.include_block_changed = false,
				.include_chunk_generated = true,
				.include_chunk_loaded = false,
				.include_chunk_unloaded = false,
				.include_entity_changed = false,
				.include_region_saved = false
			};
		}

		[[nodiscard]] static constexpr WorldEventFilter ChunkLifecycleOnly() noexcept
		{
			return WorldEventFilter{
				.include_block_changed = false,
				.include_chunk_generated = false,
				.include_chunk_loaded = true,
				.include_chunk_unloaded = true,
				.include_entity_changed = false,
				.include_region_saved = false
			};
		}

		[[nodiscard]] static constexpr WorldEventFilter EntityChangesOnly() noexcept
		{
			return WorldEventFilter{
				.include_block_changed = false,
				.include_chunk_generated = false,
				.include_chunk_loaded = false,
				.include_chunk_unloaded = false,
				.include_entity_changed = true,
				.include_region_saved = false
			};
		}

		[[nodiscard]] static constexpr WorldEventFilter RegionPersistenceOnly() noexcept
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
