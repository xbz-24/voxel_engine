#include "World.h"

#include <utility>

namespace ve::world
{
	/// Moves pending world events out of the world.
	std::vector<WorldEvent> World::DrainEvents()
	{
		return DrainEvents(WorldEventFilter::All());
	}

	/// Moves matching pending world events out of the world.
	std::vector<WorldEvent> World::DrainEvents(const WorldEventFilter& filter)
	{
		std::vector<WorldEvent> drainedEvents;
		std::vector<WorldEvent> retainedEvents;
		retainedEvents.reserve(_pendingEvents.size());
		for (WorldEvent& pendingEvent : _pendingEvents)
		{
			if (filter.Includes(pendingEvent.Type()))
			{
				drainedEvents.push_back(std::move(pendingEvent));
			}
			else
			{
				retainedEvents.push_back(std::move(pendingEvent));
			}
		}
		_pendingEvents = std::move(retainedEvents);
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
		_pendingEvents.push_back(WorldEvent{ ChunkGeneratedEvent{ chunkX, chunkZ } });
	}

	/// Records a block changed event.
	void World::RecordBlockChanged(const glm::ivec3& position, ve::blocks::BlockId previousBlockId, ve::blocks::BlockId newBlockId)
	{
		_pendingEvents.push_back(WorldEvent{ BlockChangedEvent{ position, previousBlockId, newBlockId } });
	}
}
