#pragma once

#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"
#include "ChunkTerrain.h"
#include "ChunkTypes.h"

#include <cstdint>
#include <memory>
#include <span>

class Chunk
{
public:
	static constexpr int CHUNK_WIDTH = ve::world::terrain::ChunkWidth;
	static constexpr int CHUNK_HEIGHT = ve::world::terrain::ChunkHeight;
	static constexpr int CHUNK_DEPTH = ve::world::terrain::ChunkDepth;

	Chunk(
		int chunk_coordinate_x,
		int chunk_coordinate_z,
		ChunkGenerationMode generation_mode = ChunkGenerationMode::GenerateNow,
		const ve::world::TerrainGenerationSettings& terrain_generation = {},
		std::unique_ptr<ve::rendering::RenderMesh> render_mesh_resource = nullptr) noexcept;
	~Chunk() noexcept;

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;
	Chunk(Chunk&& other) noexcept;
	Chunk& operator=(Chunk&& other) noexcept;

	// Terrain content and provenance.
	void Generate(const ve::world::TerrainGenerationSettings& terrain_generation = {}) noexcept;
	bool ReplaceBlocks(std::span<const ve::blocks::BlockId> generated_blocks);
	bool IsGenerated() const noexcept;
	bool HasProceduralTerrain() const noexcept;
	bool HasAuthoredEdits() const noexcept;
	ChunkContentProvenance Provenance() const noexcept;

	// CPU-to-GPU mesh lifecycle.
	bool TryReserveMeshBuild() noexcept;
	bool HasPendingMeshBuildReservation() const noexcept;
	void CancelMeshBuildReservation() noexcept;
	void BuildMesh(
		const ve::blocks::BlockRegistry& block_registry,
		const ve::world::mesh::NeighborChunks& neighbors);
	void UploadMesh(ve::world::mesh::ChunkMeshBuildResult mesh_build_result);
	const ve::rendering::ChunkGpuMesh& RenderMesh() const noexcept;
	ve::world::mesh::ChunkMeshInput CreateMeshInput() const noexcept;
	bool NeedsMeshBuild() const noexcept;

	// Coordinates and block storage.
	int GetChunkX() const noexcept;
	int GetChunkZ() const noexcept;
	ve::blocks::BlockId GetBlock(int local_block_x, int local_block_y, int local_block_z) const;
	bool SetBlock(int local_block_x, int local_block_y, int local_block_z, ve::blocks::BlockId block_id);
	void MarkDirty() noexcept;
	[[nodiscard]] std::uint64_t MeshRevision() const noexcept;

private:
	bool ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z) const;

	ve::world::terrain::BlockStorage blocks_;
	ve::rendering::ChunkGpuMesh mesh_;
	int chunk_x_;
	int chunk_z_;
	std::uint64_t mesh_revision_;
	bool is_mesh_built_;
	bool is_generated_;
	bool is_mesh_build_queued_;
	bool has_procedural_terrain_;
	bool has_authored_edits_;
};
