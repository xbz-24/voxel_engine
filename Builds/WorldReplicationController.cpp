#include "WorldReplicationController.h"

#include "World.h"

namespace ve::network
{
	/// Starts hosting through the underlying network session.
	bool WorldReplicationController::HostGame(const NetworkHostSettings& settings)
	{
		return session_.HostGame(settings);
	}

	/// Joins a remote host through the underlying network session.
	bool WorldReplicationController::JoinGame(const NetworkJoinSettings& settings)
	{
		return session_.JoinGame(settings);
	}

	/// Stops networking and returns to offline mode.
	void WorldReplicationController::Stop()
	{
		session_.Stop();
	}

	/// Publishes local events and applies remote messages for one frame.
	WorldReplicationFrameStats WorldReplicationController::SynchronizeWorld(ve::world::World& world)
	{
		WorldReplicationFrameStats stats;
		stats.local_messages_published = session_.PublishWorldEvents(world.DrainEvents());
		stats.incoming_stats = session_.ApplyIncomingMessages(world);
		if (stats.incoming_stats.blockMutationsApplied > 0)
		{
			stats.remote_events_suppressed = world.DrainEvents().size();
		}
		return stats;
	}

	/// Reports whether the underlying session is online.
	bool WorldReplicationController::IsOnline() const noexcept
	{
		return session_.IsOnline();
	}

	/// Returns the current high-level network mode.
	NetworkSessionMode WorldReplicationController::Mode() const noexcept
	{
		return session_.Mode();
	}
}
