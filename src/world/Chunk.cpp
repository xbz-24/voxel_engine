#include "Chunk.h"

#include "ChunkTerrain.h"

#include <algorithm>
#include <utility>

using ve::blocks::BlockId;
Chunk::Chunk(
	int chunk_coordinate_x,
	int chunk_coordinate_z,
	ChunkGenerationMode generation_mode,
	const ve::world::TerrainGenerationSettings& terrain_generation)
	: chunk_x_(chunk_coordinate_x),
	  chunk_z_(chunk_coordinate_z),
	  mesh_revision_(0),
	  is_mesh_built_(false),
	  is_generated_(false),
	  is_mesh_build_queued_(false)
{
	if (generation_mode == ChunkGenerationMode::GenerateNow) Generate(terrain_generation);
	else std::fill(&blocks_[0][0][0], &blocks_[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), BlockId::Air);
}
Chunk::~Chunk() = default;
Chunk::Chunk(Chunk&& other) noexcept
	: mesh_(std::move(other.mesh_)),
	  chunk_x_(other.chunk_x_),
	  chunk_z_(other.chunk_z_),
	  mesh_revision_(other.mesh_revision_),
	  is_mesh_built_(other.is_mesh_built_),
	  is_generated_(other.is_generated_),
	  is_mesh_build_queued_(other.is_mesh_build_queued_)
{
	std::copy(&other.blocks_[0][0][0], &other.blocks_[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks_[0][0][0]);
	other.is_mesh_built_ = false;
}
Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	mesh_ = std::move(other.mesh_);
	chunk_x_ = other.chunk_x_;
	chunk_z_ = other.chunk_z_;
	mesh_revision_ = other.mesh_revision_;
	is_mesh_built_ = other.is_mesh_built_;
	is_generated_ = other.is_generated_;
	is_mesh_build_queued_ = other.is_mesh_build_queued_;
	std::copy(&other.blocks_[0][0][0], &other.blocks_[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks_[0][0][0]);
	other.is_mesh_built_ = false;
	return *this;
}
int Chunk::GetChunkX() const noexcept
{
	return chunk_x_;
}
int Chunk::GetChunkZ() const noexcept
{
	return chunk_z_;
}
BlockId Chunk::GetBlock(int local_block_x, int local_block_y, int local_block_z) const
{
	if (!ContainsLocalBlock(local_block_x, local_block_y, local_block_z))
	{
		return BlockId::Air;
	}
	return blocks_[local_block_x][local_block_y][local_block_z];
}
bool Chunk::SetBlock(int local_block_x, int local_block_y, int local_block_z, BlockId block_id)
{
	if (!ContainsLocalBlock(local_block_x, local_block_y, local_block_z) ||
		blocks_[local_block_x][local_block_y][local_block_z] == block_id)
	{
		return false;
	}

	blocks_[local_block_x][local_block_y][local_block_z] = block_id;
	MarkDirty();
	return true;
}
void Chunk::MarkDirty()
{
	++mesh_revision_;
	is_mesh_built_ = false;
	is_mesh_build_queued_ = false;
}
std::uint64_t Chunk::MeshRevision() const noexcept
{
	return mesh_revision_;
}
bool Chunk::ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z) const
{
	return local_block_x >= 0 && local_block_x < CHUNK_WIDTH &&
		local_block_y >= 0 && local_block_y < CHUNK_HEIGHT &&
		local_block_z >= 0 && local_block_z < CHUNK_DEPTH;
}
