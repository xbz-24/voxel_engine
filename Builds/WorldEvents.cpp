#include "World.h"

namespace ve::world
{
	/// Moves pending world events out of the world.
	std::vector<WorldEvent> World::DrainEvents()
	{
		std::vector<WorldEvent> drainedEvents;
		drainedEvents.swap(_pendingEvents);
		return drainedEvents;
	}

	/// Reports pending events without draining them.
	std::size_t World::PendingEventCount() const noexcept
	{
		return _pendingEvents.size();
	}

	/// Records a generated chunk event.
	void World::RecordChunkGenerated(int chunkX, int chunkZ)
	{
		_pendingEvents.push_back(WorldEvent{
			WorldEventType::ChunkGenerated,
			BlockChangedEvent{},
			ChunkGeneratedEvent{ chunkX, chunkZ }
		});
	}

	/// Records a block changed event.
	void World::RecordBlockChanged(const glm::ivec3& position, ve::blocks::BlockId previousBlockId, ve::blocks::BlockId newBlockId)
	{
		_pendingEvents.push_back(WorldEvent{
			WorldEventType::BlockChanged,
			BlockChangedEvent{ position, previousBlockId, newBlockId },
			ChunkGeneratedEvent{}
		});
	}
}
