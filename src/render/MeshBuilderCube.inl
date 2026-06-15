#pragma once

namespace ve::rendering
{
	/** Adds six indexed quads representing an axis-aligned cube. */
	template <typename VertexAllocator, typename IndexAllocator>
	BasicMeshBuilder<VertexAllocator, IndexAllocator>&
		BasicMeshBuilder<VertexAllocator, IndexAllocator>::AddCube(
			glm::vec3 center, glm::vec3 size, ColorRgba color)
	{
		const glm::vec3 half_size = size * 0.5f;
		const glm::vec3 minimum = center - half_size;
		const glm::vec3 maximum = center + half_size;
		const std::array<glm::vec3, 8> corners{ {
			{ minimum.x, minimum.y, minimum.z }, { maximum.x, minimum.y, minimum.z },
			{ maximum.x, maximum.y, minimum.z }, { minimum.x, maximum.y, minimum.z },
			{ minimum.x, minimum.y, maximum.z }, { maximum.x, minimum.y, maximum.z },
			{ maximum.x, maximum.y, maximum.z }, { minimum.x, maximum.y, maximum.z } } };
		AddQuad(corners[0], corners[1], corners[2], corners[3], color);
		AddQuad(corners[4], corners[7], corners[6], corners[5], color);
		AddQuad(corners[0], corners[4], corners[5], corners[1], color);
		AddQuad(corners[1], corners[5], corners[6], corners[2], color);
		AddQuad(corners[2], corners[6], corners[7], corners[3], color);
		AddQuad(corners[3], corners[7], corners[4], corners[0], color);
		return *this;
	}
}
