#pragma once

#include "Block.h"
#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"
#include "ChunkTerrain.h"

#include <cstdint>
#include <memory>
#include <span>

enum class ChunkGenerationMode
{
	GenerateNow,
	Empty
};

enum class ChunkContentProvenance
{
	Empty,
	ProceduralTerrain,
	AuthoredEdits,
	ProceduralTerrainWithAuthoredEdits
};

class Chunk
{
public:
	static constexpr int CHUNK_WIDTH = ve::world::terrain::ChunkWidth;
	static constexpr int CHUNK_HEIGHT = ve::world::terrain::ChunkHeight;
	static constexpr int CHUNK_DEPTH = ve::world::terrain::ChunkDepth;

	/// Creates a chunk at chunk-grid coordinates.
	Chunk(
		int chunk_coordinate_x,
		int chunk_coordinate_z,
		ChunkGenerationMode generation_mode = ChunkGenerationMode::GenerateNow,
		const ve::world::TerrainGenerationSettings& terrain_generation = {},
		std::unique_ptr<ve::rendering::RenderMesh> render_mesh_resource = nullptr);

	/// Releases the uploaded GPU mesh if one was built.
	~Chunk();

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	/// Moves chunk CPU data and transfers ownership of the GPU mesh.
	Chunk(Chunk&& other) noexcept;

	/// Moves chunk CPU data and transfers ownership of the GPU mesh.
	Chunk& operator=(Chunk&& other) noexcept;

	/// Fills block data using procedural terrain generation.
	void Generate(const ve::world::TerrainGenerationSettings& terrain_generation = {});

	/**
	 * Replaces all local block data with generated terrain.
	 *
	 * @param generated_blocks Flat block data in chunk-local x/y/z order.
	 * @return True when the input size matched this chunk.
	 */
	bool ReplaceBlocks(std::span<const ve::blocks::BlockId> generated_blocks);

	/** @return True when this chunk contains generated terrain data. */
	bool IsGenerated() const noexcept;

	/** @return True when this chunk contains procedural terrain data. */
	bool HasProceduralTerrain() const noexcept;

	/** @return True when this chunk contains authored block edits after generation/load. */
	bool HasAuthoredEdits() const noexcept;

	/** @return Combined content provenance for serialization and tooling. */
	ChunkContentProvenance Provenance() const noexcept;

	/** @return True when this chunk was reserved for one async mesh task. */
	bool TryReserveMeshBuild() noexcept;

	/** @return True when this chunk is waiting for an already queued async mesh task. */
	bool HasPendingMeshBuildReservation() const noexcept;

	/** Releases a mesh reservation when the async task could not be queued. */
	void CancelMeshBuildReservation() noexcept;

	/**
	 * Builds a GPU mesh containing visible chunk faces.
	 *
	 * @param block_registry Registry used to resolve texture ids.
	 * @param neighbors Adjacent chunks used for border occlusion.
	 */
	void BuildMesh(const ve::blocks::BlockRegistry& block_registry, const ve::world::mesh::NeighborChunks& neighbors);

	/**
	 * Uploads finished CPU mesh data to this chunk's GPU buffer.
	 *
	 * @param mesh_build_result CPU vertices and texture batches to move/upload.
	 */
	void UploadMesh(ve::world::mesh::ChunkMeshBuildResult mesh_build_result);

	/** @return Backend-neutral uploaded chunk mesh resource. */
	const ve::rendering::ChunkGpuMesh& RenderMesh() const noexcept;

	/**
	 * Returns a read-only mesh input view over this chunk's block storage.
	 *
	 * @return Non-owning view valid while this chunk is alive and unchanged.
	 */
	ve::world::mesh::ChunkMeshInput CreateMeshInput() const noexcept;

	/**
	 * Reports whether this chunk needs a fresh mesh.
	 *
	 * @return True when terrain/block changes invalidated the GPU mesh.
	 */
	bool NeedsMeshBuild() const noexcept;

	/// Returns the chunk-grid X coordinate.
	int GetChunkX() const noexcept;

	/// Returns the chunk-grid Z coordinate.
	int GetChunkZ() const noexcept;

	/// Reads a block id, treating out-of-bounds as air.
	ve::blocks::BlockId GetBlock(int local_block_x, int local_block_y, int local_block_z) const;

	/// Writes a block id and marks the mesh dirty when the block changes.
	bool SetBlock(int local_block_x, int local_block_y, int local_block_z, ve::blocks::BlockId block_id);

	/// Marks the cached mesh as dirty so it is rebuilt next draw.
	void MarkDirty();

	/** @return Monotonic revision used to reject stale async mesh outputs. */
	[[nodiscard]] std::uint64_t MeshRevision() const noexcept;

private:
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

	/// Checks if a local coordinate belongs to this chunk.
	bool ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z) const;
};
