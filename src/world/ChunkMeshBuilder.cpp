#include "ChunkMeshBuilder.h"

#include "Chunk.h"
#include "ChunkFaceEmitter.h"
#include "ChunkMeshCollector.h"

#include <algorithm>
#include <optional>

namespace ve::world::mesh
{
	namespace
	{
		/**
		 * Orders faces so a chunk performs fewer texture binds while drawing.
		 */
		void SortFacesByTexture(std::vector<MeshFace>& faces)
		{
			std::sort(faces.begin(), faces.end(), [](const MeshFace& left, const MeshFace& right)
			{
				return left.texture < right.texture;
			});
		}

		/**
		 * Appends one texture batch to the result when the current range is non-empty.
		 */
		void AppendBatch(ChunkMeshBuildResult& result, ve::rendering::TextureHandle texture, std::uint32_t first_vertex)
		{
			const std::uint32_t vertex_count = static_cast<std::uint32_t>(result.vertices.size()) - first_vertex;
			if (vertex_count > 0)
			{
				result.batches.push_back(ve::rendering::ChunkMeshBatch{ texture, first_vertex, vertex_count });
			}
		}
	}

	ChunkMeshBuildResult BuildChunkMesh(const ChunkMeshInput& meshInput, const ve::blocks::BlockRegistry& blockRegistry, const NeighborMeshInputs& neighbors)
	{
		std::vector<MeshFace> faces;
		CollectChunkFaces(meshInput, blockRegistry, neighbors, faces);
		SortFacesByTexture(faces);

		ChunkMeshBuildResult result;
		result.vertices.reserve(faces.size() * 4);
		if (faces.empty())
		{
			return result;
		}

		ve::rendering::TextureHandle currentTexture = faces.front().texture;
		std::uint32_t batchStart = 0;
		for (const MeshFace& face : faces)
		{
			if (face.texture != currentTexture)
			{
				AppendBatch(result, currentTexture, batchStart);
				currentTexture = face.texture;
				batchStart = static_cast<std::uint32_t>(result.vertices.size());
			}
			AppendFaceVertices(face, result.vertices);
		}
		AppendBatch(result, currentTexture, batchStart);
		return result;
	}

	ChunkMeshBuildResult BuildChunkMesh(const Chunk& chunk, const ve::blocks::BlockRegistry& blockRegistry, const NeighborChunks& neighbors)
	{
		const ChunkMeshInput chunkInput = chunk.CreateMeshInput();
		const std::optional<ChunkMeshInput> west = neighbors.west ? std::optional(neighbors.west->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> east = neighbors.east ? std::optional(neighbors.east->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> north = neighbors.north ? std::optional(neighbors.north->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> south = neighbors.south ? std::optional(neighbors.south->CreateMeshInput()) : std::nullopt;
		return BuildChunkMesh(chunkInput, blockRegistry, NeighborMeshInputs{
			west ? &*west : nullptr,
			east ? &*east : nullptr,
			north ? &*north : nullptr,
			south ? &*south : nullptr
		});
	}
}
