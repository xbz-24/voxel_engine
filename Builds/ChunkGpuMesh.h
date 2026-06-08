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
		ChunkGpuMesh();
		~ChunkGpuMesh();

		ChunkGpuMesh(const ChunkGpuMesh&) = delete;
		ChunkGpuMesh& operator=(const ChunkGpuMesh&) = delete;
		ChunkGpuMesh(ChunkGpuMesh&& other) noexcept;
		ChunkGpuMesh& operator=(ChunkGpuMesh&& other) noexcept;

		void Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches);
		void Draw() const;
		void Release();

	private:
		GLuint _vertexBuffer;
		GLsizei _vertexCount;
		std::vector<ChunkMeshBatch> _batches;
	};
}

