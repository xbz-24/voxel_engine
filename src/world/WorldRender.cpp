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
		for (const ChunkRenderItem& item : ExtractVisibleChunks(request))
		{
			if (item.mesh != nullptr) item.mesh->Draw();
		}
	}

	ChunkRenderItemList World::ExtractVisibleChunks(const WorldRenderRequest& request) const
	{
		ChunkRenderItemList items;
		const ChunkViewRange range = BuildChunkViewRange(request.camera_position, _worldSize, request.render_distance_chunks);
		if (!HasChunks(range))
		{
			return items;
		}

		const visibility::ChunkVisibilityCuller culler(request);
		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				if (culler.IsChunkVisible(chunkX, chunkZ))
				{
					const Chunk* chunk = FindChunk(chunkX, chunkZ);
					if (chunk != nullptr)
					{
						items.push_back(ChunkRenderItem{ &chunk->RenderMesh(), ChunkRenderId{ chunkX, chunkZ }, chunk->MeshRevision() });
					}
				}
			}
		}
		return items;
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
