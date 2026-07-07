#include "ChunkMeshBuilder.h"

#include "Chunk.h"
#include "ChunkFaceEmitter.h"
#include "ChunkMeshCollector.h"

#include <algorithm>
#include <optional>
#include <utility>

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
				return left.material.texture < right.material.texture;
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

		class ChunkMeshAssembler
		{
		public:
			ChunkMeshAssembler(std::vector<MeshFace> faces, ChunkMeshBuildDiagnostics diagnostics)
				: faces_(std::move(faces)), diagnostics_(diagnostics)
			{
				SortFacesByTexture(faces_);
			}

			[[nodiscard]] ChunkMeshBuildResult Build() const
			{
				ChunkMeshBuildResult result;
				result.diagnostics = diagnostics_;
				result.vertices.reserve(faces_.size() * 4);
				if (faces_.empty())
				{
					return result;
				}

				ve::rendering::TextureHandle current_texture = faces_.front().material.texture;
				std::uint32_t batch_start = 0;
				for (const MeshFace& face : faces_)
				{
					if (face.material.texture != current_texture)
					{
						AppendBatch(result, current_texture, batch_start);
						current_texture = face.material.texture;
						batch_start = static_cast<std::uint32_t>(result.vertices.size());
					}
					AppendFaceVertices(face, result.vertices);
				}
				AppendBatch(result, current_texture, batch_start);
				result.diagnostics.vertex_count = result.vertices.size();
				result.diagnostics.batch_count = result.batches.size();
				return result;
			}

		private:
			std::vector<MeshFace> faces_;
			ChunkMeshBuildDiagnostics diagnostics_;
		};
	}

	ChunkMeshBuildResult BuildChunkMesh(
		const ChunkMeshInput& mesh_input,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborMeshInputs& neighbors)
	{
		std::vector<MeshFace> faces;
		ChunkMeshBuildDiagnostics diagnostics;
		CollectChunkFaces(mesh_input, block_registry, neighbors, faces, &diagnostics);
		ChunkMeshAssembler chunk_mesh_assembler(std::move(faces), diagnostics);
		return chunk_mesh_assembler.Build();
	}

	ChunkMeshBuildResult BuildChunkMesh(
		const Chunk& chunk,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborChunks& neighbors)
	{
		const ChunkMeshInput chunk_mesh_input = chunk.CreateMeshInput();
		const std::optional<ChunkMeshInput> west_neighbor_input =
			neighbors.west ? std::optional(neighbors.west->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> east_neighbor_input =
			neighbors.east ? std::optional(neighbors.east->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> north_neighbor_input =
			neighbors.north ? std::optional(neighbors.north->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> south_neighbor_input =
			neighbors.south ? std::optional(neighbors.south->CreateMeshInput()) : std::nullopt;
		return BuildChunkMesh(chunk_mesh_input, block_registry, NeighborMeshInputs{
			west_neighbor_input ? &*west_neighbor_input : nullptr,
			east_neighbor_input ? &*east_neighbor_input : nullptr,
			north_neighbor_input ? &*north_neighbor_input : nullptr,
			south_neighbor_input ? &*south_neighbor_input : nullptr
		});
	}
}
