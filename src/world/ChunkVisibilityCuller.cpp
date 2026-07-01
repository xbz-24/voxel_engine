#include "ChunkVisibilityCuller.h"

#include "ChunkTerrain.h"
#include "WorldVisibility.h"

namespace ve::world::visibility
{
	ChunkVisibilityCuller::ChunkVisibilityCuller(const WorldRenderRequest& request)
		: request_(request), frustum_(ve::rendering::Frustum::FromViewProjection(request.view_projection))
	{
	}

	bool ChunkVisibilityCuller::IsChunkVisible(int chunk_x, int chunk_z) const
	{
		if (!IntersectsFrustum(chunk_x, chunk_z)) return false;
		return IsChunkInView(request_.camera_position, request_.camera_forward, chunk_x, chunk_z);
	}

	bool ChunkVisibilityCuller::IntersectsFrustum(int chunk_x, int chunk_z) const
	{
		const float min_x = static_cast<float>(chunk_x * terrain::ChunkWidth) - 0.5f;
		const float min_z = static_cast<float>(chunk_z * terrain::ChunkDepth) - 0.5f;
		const glm::vec3 minimum_corner(min_x, -0.5f, min_z);
		const glm::vec3 maximum_corner(
			min_x + static_cast<float>(terrain::ChunkWidth),
			static_cast<float>(terrain::ChunkHeight) - 0.5f,
			min_z + static_cast<float>(terrain::ChunkDepth));
		return frustum_.IntersectsAabb(minimum_corner, maximum_corner);
	}
}
