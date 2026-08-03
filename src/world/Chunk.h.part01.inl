
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

