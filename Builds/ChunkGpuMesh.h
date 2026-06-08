#pragma once

#include <GL/glew.h>
#include <vector>

namespace ve::rendering
{
	struct ChunkVertex
	{
		float x, y, z;
		float u, v;
		float r, g, b;
	};

	struct ChunkMeshBatch
	{
		GLuint texture;
		GLsizei firstVertex;
		GLsizei vertexCount;
	};

	class ChunkGpuMesh
	{
	public:
		/**
		 * Creates an empty chunk mesh with no GPU allocation.
		 */
		ChunkGpuMesh();

		/**
		 * Releases the GPU buffer owned by this mesh.
		 */
		~ChunkGpuMesh();

		ChunkGpuMesh(const ChunkGpuMesh&) = delete;
		ChunkGpuMesh& operator=(const ChunkGpuMesh&) = delete;
		/**
		 * Takes over another mesh buffer without duplicating GPU memory.
		 *
		 * @param other Source mesh being moved from.
		 */
		ChunkGpuMesh(ChunkGpuMesh&& other) noexcept;

		/**
		 * Releases this mesh and takes over another mesh buffer.
		 *
		 * @param other Source mesh being moved from.
		 * @return Reference to this mesh.
		 */
		ChunkGpuMesh& operator=(ChunkGpuMesh&& other) noexcept;

		/**
		 * Uploads CPU vertices to a static GPU buffer.
		 *
		 * @param vertices Vertex data copied into GPU memory.
		 * @param batches Texture ranges moved into this mesh.
		 */
		void Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches);

		/**
		 * Draws all texture batches from the uploaded GPU buffer.
		 */
		void Draw() const;

		/**
		 * Frees the GPU buffer and clears batch metadata.
		 */
		void Release();

	private:
		GLuint _vertexBuffer;
		GLsizei _vertexCount;
		std::vector<ChunkMeshBatch> _batches;
	};
}
