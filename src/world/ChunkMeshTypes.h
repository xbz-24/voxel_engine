#pragma once

#include "ChunkGpuMesh.h"

#include <GL/glew.h>
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

	/**
	 * CPU-side face data before it is expanded into render vertices.
	 */
	struct MeshFace
	{
		GLuint texture;
		MeshFaceDirection direction;
		float x;
		float y;
		float z;
		float r;
		float g;
		float b;
		int width = 1;
		int height = 1;
	};

	/**
	 * Mesh data ready to upload to a chunk GPU buffer.
	 */
	struct ChunkMeshBuildResult
	{
		std::vector<ve::rendering::ChunkVertex> vertices;
		std::vector<ve::rendering::ChunkMeshBatch> batches;
	};
}
