#pragma once

#include "NetworkSession.h"

namespace ve::world
{
	class World;
}

namespace ve::network
{
	struct WorldReplicationFrameStats
	{
		std::size_t local_messages_published = 0;
		NetworkPumpStats incoming_stats;
		std::size_t remote_events_suppressed = 0;
	};

	class WorldReplicationController
	{
	public:
		/** @param settings Host listen settings. @return True when hosting started. */
		bool HostGame(const NetworkHostSettings& settings);

		/** @param settings Server endpoint plus player name. @return True when joined. */
		bool JoinGame(const NetworkJoinSettings& settings);

		/** Stops networking and returns to offline mode. */
		void Stop();

		/** @param world World to publish from and apply remote messages to. @return Frame replication stats. */
		WorldReplicationFrameStats SynchronizeWorld(ve::world::World& world);

		/** @return True when the underlying session is hosting or joined. */
		bool IsOnline() const noexcept;

		/** @return Current high-level network mode. */
		NetworkSessionMode Mode() const noexcept;

	private:
		NetworkSession session_;
	};
}
