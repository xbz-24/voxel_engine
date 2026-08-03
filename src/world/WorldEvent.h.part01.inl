
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
