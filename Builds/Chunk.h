#pragma once
#include "Block.h"
#include "BlockRegistry.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Chunk
{
public:
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 128;
	static const int CHUNK_DEPTH = 16;

	/**
	 * Creates and generates a chunk at chunk-grid coordinates.
	 *
	 * @param chunkX Chunk coordinate on the X axis.
	 * @param chunkZ Chunk coordinate on the Z axis.
	 */
	Chunk(int chunkX, int chunkZ);

	/**
	 * Releases the chunk display list if one was built.
	 */
	~Chunk();

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	/**
	 * Moves chunk CPU data and transfers ownership of the OpenGL display list.
	 *
	 * @param other Source chunk being moved from.
	 */
	Chunk(Chunk&& other) noexcept;

	/**
	 * Moves chunk CPU data and transfers ownership of the OpenGL display list.
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
	 * Builds an OpenGL display list containing visible chunk faces.
	 *
	 * @param blockRegistry Registry used to resolve block textures and solidity.
	 */
	void BuildMesh(const ve::blocks::BlockRegistry& blockRegistry);

	/**
	 * Draws the chunk, building its mesh lazily on first use.
	 *
	 * @param blockRegistry Registry used if the mesh must be built.
	 */
	void Draw(const ve::blocks::BlockRegistry& blockRegistry);

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
	ve::blocks::BlockId blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
	int _chunkX;
	int _chunkZ;
	GLuint _displayListID;
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

	/**
	 * Checks whether all six neighboring blocks hide this block.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @return true when the block is completely surrounded.
	 */
	bool IsBlockObscured(int x, int y, int z, const ve::blocks::BlockRegistry& blockRegistry) const;
};

