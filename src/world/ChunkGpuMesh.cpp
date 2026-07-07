#include "ChunkGpuMesh.h"

#include <utility>

namespace ve::rendering
{
	ChunkGpuMesh::ChunkGpuMesh()
		: ChunkGpuMesh(nullptr)
	{
	}

	ChunkGpuMesh::ChunkGpuMesh(std::unique_ptr<RenderMesh> render_mesh_resource) noexcept
		: _mesh(std::move(render_mesh_resource))
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
			cpu_vertices_ = std::move(other.cpu_vertices_);
			cpu_batches_ = std::move(other.cpu_batches_);
		}
		return *this;
	}

	void ChunkGpuMesh::Upload(const std::vector<ChunkVertex>& vertices, std::vector<ChunkMeshBatch> batches)
	{
		cpu_vertices_ = vertices;
		cpu_batches_ = std::move(batches);
		if (_mesh != nullptr) _mesh->Upload(CpuMesh());
	}

	MeshDescription ChunkGpuMesh::CpuMesh() const noexcept
	{
		return MeshDescription{ cpu_vertices_, cpu_batches_ };
	}

	void ChunkGpuMesh::Draw() const
	{
		if (_mesh != nullptr) _mesh->Draw();
	}

	void ChunkGpuMesh::Release()
	{
		if (_mesh != nullptr) _mesh->Release();
		cpu_vertices_.clear();
		cpu_batches_.clear();
	}
}
