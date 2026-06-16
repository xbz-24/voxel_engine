#include "ChunkGpuMesh.h"

#include <cstddef>
#include <utility>

namespace ve::rendering
{
	ChunkGpuMesh::ChunkGpuMesh() : _vertexBuffer(0), _vertexCount(0) {}

	ChunkGpuMesh::~ChunkGpuMesh()
	{
		Release();
	}

	ChunkGpuMesh::ChunkGpuMesh(ChunkGpuMesh&& other) noexcept
		: _vertexBuffer(other._vertexBuffer), _vertexCount(other._vertexCount), _batches(std::move(other._batches))
	{
		other._vertexBuffer = 0;
		other._vertexCount = 0;
	}

	ChunkGpuMesh& ChunkGpuMesh::operator=(ChunkGpuMesh&& other) noexcept
	{
		if (this != &other)
		{
			Release();
			_vertexBuffer = other._vertexBuffer;
			_vertexCount = other._vertexCount;
			_batches = std::move(other._batches);
			other._vertexBuffer = 0;
			other._vertexCount = 0;
		}
		return *this;
	}

	void ChunkGpuMesh::Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches)
	{
		Release();
		if (vertices.empty())
		{
			return;
		}

		glGenBuffers(1, &_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(ChunkVertex)), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_vertexCount = static_cast<GLsizei>(vertices.size());
		_batches = std::move(batches);
	}

	void ChunkGpuMesh::Draw() const
	{
		if (_vertexBuffer == 0 || _vertexCount == 0)
		{
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, x)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, u)));
		glColorPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, r)));

		for (const ChunkMeshBatch& batch : _batches)
		{
			glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(batch.texture));
			glDrawArrays(GL_QUADS, static_cast<GLint>(batch.first_vertex), static_cast<GLsizei>(batch.vertex_count));
		}

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	void ChunkGpuMesh::Release()
	{
		if (_vertexBuffer != 0)
		{
			glDeleteBuffers(1, &_vertexBuffer);
			_vertexBuffer = 0;
		}
		_vertexCount = 0;
		_batches.clear();
	}
}
