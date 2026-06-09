#include "ChunkMeshCollector.h"
#include <array>

using ve::blocks::BlockFace;
using ve::blocks::BlockId;
namespace ve::world::mesh
{
	namespace
	{
		struct FaceProbe
		{
			BlockFace face;
			MeshFaceDirection direction;
			int x, y, z;
		};
		constexpr std::array<FaceProbe, 6> FaceProbes{{
			{BlockFace::Top, MeshFaceDirection::Top, 0, 1, 0},
			{BlockFace::Bottom, MeshFaceDirection::Bottom, 0, -1, 0},
			{BlockFace::Front, MeshFaceDirection::Front, 0, 0, 1},
			{BlockFace::Back, MeshFaceDirection::Back, 0, 0, -1},
			{BlockFace::Right, MeshFaceDirection::Right, 1, 0, 0},
			{BlockFace::Left, MeshFaceDirection::Left, -1, 0, 0}
		}};
		/**
		 * Reads a block from this input or from one of its four neighbors.
		 */
		BlockId ReadBlock(const ChunkMeshInput& meshInput, const NeighborMeshInputs& neighbors, int x, int y, int z)
		{
			if (y < 0 || y >= terrain::ChunkHeight)
			{
				return BlockId::Air;
			}
			if (x < 0)
			{
				return neighbors.west ? neighbors.west->GetBlock(terrain::ChunkWidth - 1, y, z) : BlockId::Air;
			}
			if (x >= terrain::ChunkWidth)
			{
				return neighbors.east ? neighbors.east->GetBlock(0, y, z) : BlockId::Air;
			}
			if (z < 0)
			{
				return neighbors.north ? neighbors.north->GetBlock(x, y, terrain::ChunkDepth - 1) : BlockId::Air;
			}
			if (z >= terrain::ChunkDepth)
			{
				return neighbors.south ? neighbors.south->GetBlock(x, y, 0) : BlockId::Air;
			}
			return meshInput.GetBlock(x, y, z);
		}
		/**
		 * Adds one visible face with a light tint baked into vertices.
		 */
		void PushFace(std::vector<MeshFace>& faces, const ve::blocks::BlockRegistry& registry, BlockId blockId, const FaceProbe& probe, float x, float y, float z)
		{
			const bool isGrassTop = blockId == BlockId::Grass && probe.face == BlockFace::Top;
			faces.push_back(MeshFace{ registry.TextureFor(blockId, probe.face), probe.direction, x, y, z, isGrassTop ? 0.404f : 1.0f, isGrassTop ? 0.655f : 1.0f, isGrassTop ? 0.239f : 1.0f });
		}
	}
	void CollectChunkFaces(const ChunkMeshInput& meshInput, const ve::blocks::BlockRegistry& blockRegistry, const NeighborMeshInputs& neighbors, std::vector<MeshFace>& faces)
	{
		faces.reserve(terrain::ChunkWidth * terrain::ChunkDepth * 8);
		for (int x = 0; x < terrain::ChunkWidth; x++)
		{
			for (int z = 0; z < terrain::ChunkDepth; z++)
			{
				for (int y = 0; y < terrain::ChunkHeight; y++)
				{
					const BlockId blockId = meshInput.GetBlock(x, y, z);
					if (blockRegistry.IsAir(blockId))
					{
						continue;
					}

					const float worldX = static_cast<float>(x + (meshInput.chunkX * terrain::ChunkWidth));
					const float worldY = static_cast<float>(y);
					const float worldZ = static_cast<float>(z + (meshInput.chunkZ * terrain::ChunkDepth));
					for (const FaceProbe& probe : FaceProbes)
					{
						const BlockId neighbor = ReadBlock(meshInput, neighbors, x + probe.x, y + probe.y, z + probe.z);
						if (!blockRegistry.IsSolid(neighbor))
						{
							PushFace(faces, blockRegistry, blockId, probe, worldX, worldY, worldZ);
						}
					}
				}
			}
		}
	}
}
