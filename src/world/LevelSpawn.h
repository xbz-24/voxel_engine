#pragma once

#include "MemoryArena.h"

#include <cstddef>
#include <memory_resource>

/**
 * Owns the memory arena used while a level is alive.
 *
 * The idea is to reserve one large backing block at level startup and then hand
 * allocators from that block to containers such as std::vector. This avoids many
 * small heap allocations while spawning world data.
 */
class LevelSpawn
{
public:
	/**
	 * Creates a level memory arena.
	 *
	 * @param byteCapacity Number of bytes reserved for level-lifetime allocations.
	 */
	explicit LevelSpawn(std::size_t byteCapacity);

	LevelSpawn(const LevelSpawn&) = delete;
	LevelSpawn& operator=(const LevelSpawn&) = delete;
	LevelSpawn(LevelSpawn&&) = delete;
	LevelSpawn& operator=(LevelSpawn&&) = delete;

	/**
	 * Returns the memory resource used by level containers.
	 *
	 * @return Polymorphic memory resource backed by this level arena.
	 */
	std::pmr::memory_resource& MemoryResource() noexcept;

private:
	ve::memory::MemoryArena _arena;
};
