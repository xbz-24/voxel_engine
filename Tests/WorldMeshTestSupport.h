#pragma once

#include "ChunkMeshInput.h"
#include "CoreTypes.h"
#include "RenderBackend.h"

#include <memory>
#include <vector>

class RecordingRenderMesh final : public ve::rendering::RenderMesh
{
public:
	void Upload(const ve::rendering::MeshDescription& description) override
	{
		uploaded_vertex_count = description.vertices.size();
		uploaded_batch_count = description.batches.size();
		++upload_call_count;
	}

	void Draw() const override { ++draw_call_count; }
	void Release() noexcept override { ++release_call_count; }

	std::size_t uploaded_vertex_count = 0;
	std::size_t uploaded_batch_count = 0;
	int upload_call_count = 0;
	mutable int draw_call_count = 0;
	int release_call_count = 0;
};

class CountingRenderBackend final : public ve::rendering::RenderBackend
{
public:
	[[nodiscard]] ve::rendering::GraphicsApi Api() const noexcept override
	{
		return ve::rendering::GraphicsApi::OpenGLCompatibility;
	}

	[[nodiscard]] ve::rendering::RenderBackendCapabilities Capabilities() const noexcept override
	{
		return ve::rendering::RenderBackendCapabilities{ .is_available = true };
	}

	[[nodiscard]] std::unique_ptr<ve::rendering::RenderMesh> CreateMeshResource() const override
	{
		++created_mesh_resource_count;
		return std::make_unique<RecordingRenderMesh>();
	}

	mutable int created_mesh_resource_count = 0;
};

[[nodiscard]] inline std::size_t LocalBlockIndex(int x, int y, int z)
{
	return ve::core::ToIndex(
		(x * ve::world::terrain::ChunkHeight + y) * ve::world::terrain::ChunkDepth + z);
}

inline void SetLocalBlock(
	std::vector<ve::blocks::BlockId>& blocks,
	int x,
	int y,
	int z,
	ve::blocks::BlockId block_id)
{
	blocks[LocalBlockIndex(x, y, z)] = block_id;
}
