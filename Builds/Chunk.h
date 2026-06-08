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

	/**
	 * Creates and generates a chunk at chunk-grid coordinates.
	 *
	 * @param chunkX Chunk coordinate on the X axis.
	 * @param chunkZ Chunk coordinate on the Z axis.
	 */
	Chunk(int chunkX, int chunkZ);

	/**
	 * Releases the uploaded GPU mesh if one was built.
	 */
	~Chunk();

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	/**
	 * Moves chunk CPU data and transfers ownership of the GPU mesh.
	 *
	 * @param other Source chunk being moved from.
	 */
	Chunk(Chunk&& other) noexcept;

	/**
	 * Moves chunk CPU data and transfers ownership of the GPU mesh.
	 *
	 * @param other Source chunk being moved from.
	 * @return Reference to this chunk after assignment.
	 */
	Chunk& operator=(Chunk&& other) noexcept;

	/**
	 * Fills block data using procedural height waves.
	 */
	void Generate();

	/**
	 * Builds a GPU mesh containing visible chunk faces.
	 *
	 * @param blockRegistry Registry used to resolve block textures and solidity.
	 * @param westNeighbor Neighbor chunk at X - 1, or nullptr when absent.
	 * @param eastNeighbor Neighbor chunk at X + 1, or nullptr when absent.
	 * @param northNeighbor Neighbor chunk at Z - 1, or nullptr when absent.
	 * @param southNeighbor Neighbor chunk at Z + 1, or nullptr when absent.
	 */
	void BuildMesh(const ve::blocks::BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor);

	/**
	 * Draws the chunk, building its mesh lazily on first use.
	 *
	 * @param blockRegistry Registry used if the mesh must be built.
	 * @param westNeighbor Neighbor chunk at X - 1, or nullptr when absent.
	 * @param eastNeighbor Neighbor chunk at X + 1, or nullptr when absent.
	 * @param northNeighbor Neighbor chunk at Z - 1, or nullptr when absent.
	 * @param southNeighbor Neighbor chunk at Z + 1, or nullptr when absent.
	 */
	void Draw(const ve::blocks::BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor);

	/**
	 * Returns the chunk-grid X coordinate.
	 *
	 * @return Chunk X coordinate.
	 */
	int GetChunkX() const noexcept;

	/**
	 * Returns the chunk-grid Z coordinate.
	 *
	 * @return Chunk Z coordinate.
	 */
	int GetChunkZ() const noexcept;

	/**
	 * Reads a block id, treating out-of-bounds as air.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @return Block id, or Air for out-of-bounds.
	 */
	ve::blocks::BlockId GetBlock(int x, int y, int z) const;

	/**
	 * Writes a block id and marks the mesh dirty when the block changes.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @param blockId New block id.
	 * @return true when a block was changed.
	 */
	bool SetBlock(int x, int y, int z, ve::blocks::BlockId blockId);

	/**
	 * Marks the cached mesh as dirty so it is rebuilt next draw.
	 */
	void MarkDirty();

private:
	ve::world::terrain::BlockStorage blocks;
	ve::rendering::ChunkGpuMesh _mesh;
	int _chunkX;
	int _chunkZ;
	bool _isMeshBuilt;

	/**
	 * Checks if a local coordinate belongs to this chunk.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @return true when the coordinate is inside chunk bounds.
	 */
	bool ContainsLocalBlock(int x, int y, int z) const;

};

