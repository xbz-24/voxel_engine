#include "LevelSpawn.h"

/**
 * Creates a level memory arena.
 *
 * @param byteCapacity Number of bytes reserved for level-lifetime allocations.
 */
LevelSpawn::LevelSpawn(std::size_t byteCapacity)
	: _arena(byteCapacity)
{
}

/**
 * Returns the memory resource used by level containers.
 *
 * @return Polymorphic memory resource backed by this level arena.
 */
std::pmr::memory_resource& LevelSpawn::MemoryResource() noexcept
{
	return _arena.Resource();
}

std::size_t LevelSpawn::MemoryCapacityBytes() const noexcept
{
	return _arena.Capacity();
}

std::size_t LevelSpawn::MemoryBytesUsed() const noexcept
{
	return _arena.BytesUsed();
}
