#include "LevelSpawn.h"

/**
 * Creates a level memory arena.
 *
 * @param byteCapacity Number of bytes reserved for level-lifetime allocations.
 */
LevelSpawn::LevelSpawn(std::size_t byteCapacity)
	: _backingStore(byteCapacity),
	  _memoryResource(_backingStore.data(), _backingStore.size())
{
}

/**
 * Returns the memory resource used by level containers.
 *
 * @return Polymorphic memory resource backed by this level arena.
 */
std::pmr::memory_resource& LevelSpawn::MemoryResource() noexcept
{
	return _memoryResource;
}

