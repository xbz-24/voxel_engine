#pragma once

#include "Frustum.h"
#include "WorldRenderRequest.h"

namespace ve::world::visibility
{
	class ChunkVisibilityCuller
	{
	public:
		/**
		 * Creates a visibility culler for one render request.
		 *
		 * @param request Current world render request.
		 */
		explicit ChunkVisibilityCuller(const WorldRenderRequest& request);

		/**
		 * Checks whether a chunk should be submitted.
		 *
		 * @param chunk_x Chunk-grid X coordinate.
		 * @param chunk_z Chunk-grid Z coordinate.
		 * @return True when the chunk survives frustum and direction culling.
		 */
		bool IsChunkVisible(int chunk_x, int chunk_z) const;

	private:
		bool IntersectsFrustum(int chunk_x, int chunk_z) const;

		const WorldRenderRequest& request_;
		ve::rendering::Frustum frustum_;
	};
}
