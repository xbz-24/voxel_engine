#include "ChunkGpuMesh.h"

#include <utility>

namespace ve::rendering
{
	ChunkGpuMesh::ChunkGpuMesh()
		// TODO: Create this through the active RenderBackend so chunk storage is not implicitly OpenGL-only.
		: _mesh(CreateOpenGLRenderMesh())
	{
	}

	ChunkGpuMesh::~ChunkGpuMesh()
	{
		Release();
	}

	ChunkGpuMesh::ChunkGpuMesh(ChunkGpuMesh&& other) noexcept = default;

	ChunkGpuMesh& ChunkGpuMesh::operator=(ChunkGpuMesh&& other) noexcept
	{
		if (this != &other)
		{
			Release();
			_mesh = std::move(other._mesh);
		}
		return *this;
	}

	void ChunkGpuMesh::Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches)
	{
		// TODO: Preserve a backend-neutral CPU mesh payload here and let the renderer choose OpenGL buffers or Vulkan buffers.
		if (_mesh == nullptr) _mesh = CreateOpenGLRenderMesh();
		_mesh->Upload(MeshDescription{ vertices, batches });
	}

	void ChunkGpuMesh::Draw() const
	{
		if (_mesh != nullptr) _mesh->Draw();
	}

	void ChunkGpuMesh::Release()
	{
		if (_mesh != nullptr) _mesh->Release();
	}
}
