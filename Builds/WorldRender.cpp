#include "World.h"

#include "WorldViewRange.h"
#include "WorldVisibility.h"

namespace ve::world
{
	/// Draws chunks described by a render request after visibility culling.
	void World::Draw(const WorldRenderRequest& request)
	{
		const ChunkViewRange range = BuildChunkViewRange(request.cameraPosition, _worldSize, request.renderDistanceChunks);
		if (!HasChunks(range))
		{
			return;
		}

		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				DrawVisibleChunk(request, chunkX, chunkZ);
			}
		}
	}

	/// Draws one chunk when it passes camera-direction culling.
	void World::DrawVisibleChunk(const WorldRenderRequest& request, int chunkX, int chunkZ)
	{
		if (!visibility::IsChunkInView(request.cameraPosition, request.cameraForward, chunkX, chunkZ))
		{
			return;
		}

		Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (chunk)
		{
			chunk->Draw(request.blockRegistry, FindNeighborChunks(chunkX, chunkZ));
		}
	}

	/// Collects neighboring chunks used to hide shared border faces.
	ve::world::mesh::NeighborChunks World::FindNeighborChunks(int chunkX, int chunkZ) const
	{
		return {
			FindChunk(chunkX - 1, chunkZ),
			FindChunk(chunkX + 1, chunkZ),
			FindChunk(chunkX, chunkZ - 1),
			FindChunk(chunkX, chunkZ + 1)
		};
	}
}
