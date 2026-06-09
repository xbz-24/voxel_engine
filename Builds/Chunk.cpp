#include "Chunk.h"

#include "ChunkTerrain.h"

#include <algorithm>
#include <utility>

using ve::blocks::BlockId;
using ve::blocks::BlockRegistry;
Chunk::Chunk(int chunkX, int chunkZ)
	: _chunkX(chunkX), _chunkZ(chunkZ), _isMeshBuilt(false)
{
	Generate();
}
Chunk::~Chunk() = default;
Chunk::Chunk(Chunk&& other) noexcept
	: _mesh(std::move(other._mesh)),
	  _chunkX(other._chunkX),
	  _chunkZ(other._chunkZ),
	  _isMeshBuilt(other._isMeshBuilt)
{
	std::copy(&other.blocks[0][0][0], &other.blocks[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks[0][0][0]);
	other._isMeshBuilt = false;
}
Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	_mesh = std::move(other._mesh);
	_chunkX = other._chunkX;
	_chunkZ = other._chunkZ;
	_isMeshBuilt = other._isMeshBuilt;
	std::copy(&other.blocks[0][0][0], &other.blocks[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks[0][0][0]);
	other._isMeshBuilt = false;
	return *this;
}
void Chunk::Generate()
{
	ve::world::terrain::GenerateChunkTerrain(_chunkX, _chunkZ, blocks);
	MarkDirty();
}
int Chunk::GetChunkX() const noexcept
{
	return _chunkX;
}
int Chunk::GetChunkZ() const noexcept
{
	return _chunkZ;
}
BlockId Chunk::GetBlock(int x, int y, int z) const
{
	if (!ContainsLocalBlock(x, y, z))
	{
		return BlockId::Air;
	}
	return blocks[x][y][z];
}
bool Chunk::SetBlock(int x, int y, int z, BlockId blockId)
{
	if (!ContainsLocalBlock(x, y, z) || blocks[x][y][z] == blockId)
	{
		return false;
	}

	blocks[x][y][z] = blockId;
	MarkDirty();
	return true;
}
void Chunk::MarkDirty()
{
	_isMeshBuilt = false;
}
bool Chunk::ContainsLocalBlock(int x, int y, int z) const
{
	return x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_DEPTH;
}
