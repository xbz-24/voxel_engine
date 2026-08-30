#pragma once

#include "voxel/WorldConfiguration.h"

namespace voxel
{
	[[nodiscard]] WorldConfig World(int size_chunks);
	[[nodiscard]] WorldConfig World(ChunkCount size_chunks);
	[[nodiscard]] WorldConfig World();
	[[nodiscard]] WorldConfig Scene(int size_chunks);
	[[nodiscard]] WorldConfig Scene(ChunkCount size_chunks);
	[[nodiscard]] WorldConfig Scene();
}
