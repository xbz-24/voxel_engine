#pragma once

#include "Block.h"
#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkTerrain.h"

class Chunk
{
public:
	static constexpr int CHUNK_WIDTH = ve::world::terrain::ChunkWidth;
	static constexpr int CHUNK_HEIGHT = ve::world::terrain::ChunkHeight;
	static constexpr int CHUNK_DEPTH = ve::world::terrain::ChunkDepth;

	/// Creates and generates a chunk at chunk-grid coordinates.
	Chunk(int chunkX, int chunkZ);

	/// Releases the uploaded GPU mesh if one was built.
	~Chunk();

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	/// Moves chunk CPU data and transfers ownership of the GPU mesh.
	Chunk(Chunk&& other) noexcept;

	/// Moves chunk CPU data and transfers ownership of the GPU mesh.
	Chunk& operator=(Chunk&& other) noexcept;

	/// Fills block data using procedural terrain generation.
	void Generate();

	/// Builds a GPU mesh containing visible chunk faces.
	void BuildMesh(const ve::blocks::BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor);

	/// Draws the chunk, building its mesh lazily on first use.
	void Draw(const ve::blocks::BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor);

	/// Returns the chunk-grid X coordinate.
	int GetChunkX() const noexcept;

	/// Returns the chunk-grid Z coordinate.
	int GetChunkZ() const noexcept;

	/// Reads a block id, treating out-of-bounds as air.
	ve::blocks::BlockId GetBlock(int x, int y, int z) const;

	/// Writes a block id and marks the mesh dirty when the block changes.
	bool SetBlock(int x, int y, int z, ve::blocks::BlockId blockId);

	/// Marks the cached mesh as dirty so it is rebuilt next draw.
	void MarkDirty();

private:
	ve::world::terrain::BlockStorage blocks;
	ve::rendering::ChunkGpuMesh _mesh;
	int _chunkX;
	int _chunkZ;
	bool _isMeshBuilt;

	/// Checks if a local coordinate belongs to this chunk.
	bool ContainsLocalBlock(int x, int y, int z) const;
};
