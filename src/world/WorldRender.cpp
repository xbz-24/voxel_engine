#include "World.h"

#include "ChunkVisibilityCuller.h"
#include "WorldViewRange.h"

namespace ve::world
{
	/**
	 * Draws chunks described by a render request after visibility culling.
	 *
	 * @param request Camera and render data used to submit visible chunks.
	 */
	void World::Draw(const WorldRenderRequest& request)
	{
		const ChunkViewRange range = BuildChunkViewRange(request.cameraPosition, _worldSize, request.renderDistanceChunks);
		if (!HasChunks(range))
		{
			return;
		}

		const visibility::ChunkVisibilityCuller culler(request);
		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				if (culler.IsChunkVisible(chunkX, chunkZ))
				{
					DrawVisibleChunk(request, chunkX, chunkZ);
				}
			}
		}
	}

	/**
	 * Draws one chunk when it passes camera-direction culling.
	 *
	 * @param request Camera and render data used for culling and drawing.
	 * @param chunkX Chunk-grid X coordinate to test.
	 * @param chunkZ Chunk-grid Z coordinate to test.
	 */
	void World::DrawVisibleChunk(const WorldRenderRequest& request, int chunkX, int chunkZ)
	{
		Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (chunk)
		{
			chunk->Draw(request.blockRegistry, FindNeighborChunks(chunkX, chunkZ));
		}
	}

	/**
	 * Collects neighboring chunks used to hide shared border faces.
	 *
	 * @param chunkX Chunk-grid X coordinate for the center chunk.
	 * @param chunkZ Chunk-grid Z coordinate for the center chunk.
	 * @return Neighbor chunk pointers, using nullptr for missing neighbors.
	 */
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
