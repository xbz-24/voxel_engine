#pragma once

#include "Block.h"

#include <glm/glm.hpp>

#include <variant>

namespace ve::world
{
	/**
	 * Event categories emitted by the world simulation.
	 */
	enum class WorldEventType
	{
		// TODO: Add ChunkLoaded, ChunkUnloaded, EntityChanged, and RegionSaved events before tooling relies on polling.
		BlockChanged,
		ChunkGenerated
	};

	struct WorldEventFilter
	{
		bool include_block_changed = true;
		bool include_chunk_generated = true;

		[[nodiscard]] static constexpr WorldEventFilter All() noexcept
		{
			return {};
		}

		[[nodiscard]] static constexpr WorldEventFilter BlockChangesOnly() noexcept
		{
			return WorldEventFilter{ true, false };
		}

		[[nodiscard]] static constexpr WorldEventFilter ChunkGenerationOnly() noexcept
		{
			return WorldEventFilter{ false, true };
		}

		[[nodiscard]] constexpr bool Includes(WorldEventType type) const noexcept
		{
			return (type == WorldEventType::BlockChanged && include_block_changed) ||
				(type == WorldEventType::ChunkGenerated && include_chunk_generated);
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

	using WorldEventPayload = std::variant<BlockChangedEvent, ChunkGeneratedEvent>;

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

		[[nodiscard]] WorldEventType Type() const noexcept
		{
			if (std::holds_alternative<BlockChangedEvent>(payload)) return WorldEventType::BlockChanged;
			return WorldEventType::ChunkGenerated;
		}

		[[nodiscard]] const BlockChangedEvent* AsBlockChanged() const noexcept
		{
			return std::get_if<BlockChangedEvent>(&payload);
		}

		[[nodiscard]] const ChunkGeneratedEvent* AsChunkGenerated() const noexcept
		{
			return std::get_if<ChunkGeneratedEvent>(&payload);
		}

		WorldEventPayload payload;
	};
}
