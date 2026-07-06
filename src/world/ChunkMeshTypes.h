#pragma once

#include "GraphicsTypes.h"

#include <cstddef>
#include <vector>

class Chunk;

namespace ve::world::mesh
{
	/**
	 * Bundles the optional chunks used to hide shared border faces.
	 */
	struct NeighborChunks
	{
		const Chunk* west;
		const Chunk* east;
		const Chunk* north;
		const Chunk* south;
	};

	/**
	 * Identifies the cube side represented by a collected mesh face.
	 */
	enum class MeshFaceDirection
	{
		Top,
		Bottom,
		Front,
		Back,
		Right,
		Left
	};

	struct MeshFaceMaterialPayload
	{
		ve::rendering::TextureHandle texture = ve::rendering::kInvalidTextureHandle;
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		float light = 1.0f;

		[[nodiscard]] friend bool operator==(const MeshFaceMaterialPayload&, const MeshFaceMaterialPayload&) noexcept = default;
	};

	/**
	 * CPU-side face data before it is expanded into render vertices.
	 */
	struct MeshFace
	{
		MeshFaceMaterialPayload material{};
		MeshFaceDirection direction;
		float world_center_x;
		float world_center_y;
		float world_center_z;
		int width = 1;
		int height = 1;
	};

	/**
	 * Counts the work done while converting chunk blocks into renderable faces.
	 */
	struct ChunkMeshBuildDiagnostics
	{
		std::size_t candidate_block_face_count = 0;
		std::size_t visible_block_face_count = 0;
		std::size_t culled_block_face_count = 0;
		std::size_t merged_face_count = 0;
		std::size_t vertex_count = 0;
		std::size_t batch_count = 0;

		/** @return Average number of visible block faces represented by each merged face. */
		[[nodiscard]] float MergeRatio() const noexcept
		{
			if (merged_face_count == 0) return 0.0f;
			return static_cast<float>(visible_block_face_count) / static_cast<float>(merged_face_count);
		}
	};

	/**
	 * Mesh data ready to upload to a chunk GPU buffer.
	 */
	struct ChunkMeshBuildResult
	{
		std::vector<ve::rendering::ChunkVertex> vertices;
		std::vector<ve::rendering::ChunkMeshBatch> batches;
		ChunkMeshBuildDiagnostics diagnostics;
	};
}
