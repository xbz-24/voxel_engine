#pragma once
#include "Cube.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Chunk
{
public:
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 128;
	static const int CHUNK_DEPTH = 16;

	int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];

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
	 * @param cubeManager Cube texture provider used to bind block textures.
	 */
	void BuildMesh(Cube& cubeManager);

	/**
	 * Draws the chunk, building its mesh lazily on first use.
	 *
	 * @param cubeManager Cube texture provider used if the mesh must be built.
	 */
	void Draw(Cube& cubeManager);

private:
	int _chunkX;
	int _chunkZ;
	GLuint _displayListID;
	bool _isMeshBuilt;

	/**
	 * Reads a block id, treating out-of-bounds as air.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @return Block id, or 0 for air/out-of-bounds.
	 */
	int GetBlock(int x, int y, int z);

	/**
	 * Checks whether all six neighboring blocks hide this block.
	 *
	 * @param x Local block X coordinate.
	 * @param y Local block Y coordinate.
	 * @param z Local block Z coordinate.
	 * @return true when the block is completely surrounded.
	 */
	bool IsBlockObscured(int x, int y, int z);
};

