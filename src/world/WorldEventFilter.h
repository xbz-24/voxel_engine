#pragma once

#include "WorldEventType.h"

namespace ve::world
{
	struct WorldEventFilter
	{
		bool include_block_changed = true;
		bool include_chunk_generated = true;
		bool include_chunk_loaded = true;
		bool include_chunk_unloaded = true;
		bool include_entity_changed = true;
		bool include_region_saved = true;

		[[nodiscard]] static constexpr WorldEventFilter All() noexcept { return {}; }

		[[nodiscard]] static constexpr WorldEventFilter BlockChangesOnly() noexcept
		{
			return { true, false, false, false, false, false };
		}

		[[nodiscard]] static constexpr WorldEventFilter ChunkGenerationOnly() noexcept
		{
			return { false, true, false, false, false, false };
		}

		[[nodiscard]] static constexpr WorldEventFilter ChunkLifecycleOnly() noexcept
		{
			return { false, false, true, true, false, false };
		}

		[[nodiscard]] static constexpr WorldEventFilter EntityChangesOnly() noexcept
		{
			return { false, false, false, false, true, false };
		}

		[[nodiscard]] static constexpr WorldEventFilter RegionPersistenceOnly() noexcept
		{
			return { false, false, false, false, false, true };
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
}
