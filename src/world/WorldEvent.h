#pragma once

#include "WorldEventFilter.h"
#include "WorldEventPayload.h"

#include <utility>

namespace ve::world
{
	struct WorldEvent
	{
		explicit WorldEvent(BlockChangedEvent event) noexcept : payload(event) {}
		explicit WorldEvent(ChunkGeneratedEvent event) noexcept : payload(event) {}
		explicit WorldEvent(ChunkLoadedEvent event) noexcept : payload(event) {}
		explicit WorldEvent(ChunkUnloadedEvent event) noexcept : payload(event) {}
		explicit WorldEvent(EntityChangedEvent event) noexcept : payload(std::move(event)) {}
		explicit WorldEvent(RegionSavedEvent event) noexcept : payload(std::move(event)) {}

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
