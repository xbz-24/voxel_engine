#pragma once

namespace ve::world
{
	enum class WorldEventType
	{
		BlockChanged,
		ChunkGenerated,
		ChunkLoaded,
		ChunkUnloaded,
		EntityChanged,
		RegionSaved
	};
}
