#include "Chunk.h"
#include <algorithm>
#include <cmath>

using ve::blocks::BlockFace;
using ve::blocks::BlockId;
using ve::blocks::BlockRegistry;

namespace
{
	constexpr float BlockHalfSize = 0.5f;

	/**
	 * Emits the top face of one block into the active immediate-mode batch.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitTopFace(float dx, float dy, float dz)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
	}

	/**
	 * Emits the bottom face of one block into the active immediate-mode batch.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitBottomFace(float dx, float dy, float dz)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
	}

	/**
	 * Emits the positive Z side face of one block.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitFrontFace(float dx, float dy, float dz)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
	}

	/**
	 * Emits the negative Z side face of one block.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitBackFace(float dx, float dy, float dz)
	{
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
	}

	/**
	 * Emits the positive X side face of one block.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitRightFace(float dx, float dy, float dz)
	{
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx + BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx + BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
	}

	/**
	 * Emits the negative X side face of one block.
	 *
	 * @param dx World X block origin.
	 * @param dy World Y block origin.
	 * @param dz World Z block origin.
	 */
	void EmitLeftFace(float dx, float dy, float dz)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz - BlockHalfSize);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dx - BlockHalfSize, dy - BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz + BlockHalfSize);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - BlockHalfSize, dy + BlockHalfSize, dz - BlockHalfSize);
	}
}

Chunk::Chunk(int chunkX, int chunkZ) : _chunkX(chunkX), _chunkZ(chunkZ)
{
	_isMeshBuilt = false;
	_displayListID = 0;
	Generate();
}

Chunk::~Chunk()
{
	if (_displayListID != 0)
	{
		glDeleteLists(_displayListID, 1);
	}
}

Chunk::Chunk(Chunk&& other) noexcept
	: _chunkX(other._chunkX),
	  _chunkZ(other._chunkZ),
	  _displayListID(other._displayListID),
	  _isMeshBuilt(other._isMeshBuilt)
{
	std::copy(&other.blocks[0][0][0], &other.blocks[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks[0][0][0]);
	other._displayListID = 0;
	other._isMeshBuilt = false;
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	if (_displayListID != 0)
	{
		glDeleteLists(_displayListID, 1);
	}

	_chunkX = other._chunkX;
	_chunkZ = other._chunkZ;
	_displayListID = other._displayListID;
	_isMeshBuilt = other._isMeshBuilt;
	std::copy(&other.blocks[0][0][0], &other.blocks[0][0][0] + (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH), &blocks[0][0][0]);

	other._displayListID = 0;
	other._isMeshBuilt = false;
	return *this;
}

void Chunk::Generate()
{
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_DEPTH; z++)
		{
			float globalX = static_cast<float>(x + (_chunkX * CHUNK_WIDTH));
			float globalZ = static_cast<float>(z + (_chunkZ * CHUNK_DEPTH));

			float continent = std::sin(globalX * 0.01f) * std::cos(globalZ * 0.01f) * 30.0f;
			float mountains = std::sin(globalX * 0.05f + globalZ * 0.02f) * 15.0f;
			float hills = std::sin(globalX * 0.15f) * std::cos(globalZ * 0.15f) * 4.0f;
			int surfaceHeight = 40 + static_cast<int>(continent + mountains + hills);

			if (surfaceHeight < 1) surfaceHeight = 1;
			if (surfaceHeight >= CHUNK_HEIGHT - 1) surfaceHeight = CHUNK_HEIGHT - 2;

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (y < surfaceHeight - 4)
				{
					const int orePattern = static_cast<int>(globalX * 13.0f + globalZ * 7.0f + static_cast<float>(y) * 5.0f);
					if (y < 28 && orePattern % 97 == 0)
					{
						blocks[x][y][z] = BlockId::DiamondOre;
					}
					else if (y < 42 && orePattern % 53 == 0)
					{
						blocks[x][y][z] = BlockId::IronOre;
					}
					else if (orePattern % 37 == 0)
					{
						blocks[x][y][z] = BlockId::CoalOre;
					}
					else
					{
						blocks[x][y][z] = BlockId::Stone;
					}
				}
				else if (y < surfaceHeight)
				{
					blocks[x][y][z] = BlockId::Dirt;
				}
				else if (y == surfaceHeight)
				{
					blocks[x][y][z] = BlockId::Grass;
				}
				else
				{
					blocks[x][y][z] = BlockId::Air;
				}
			}

			if ((static_cast<int>(globalX) % 23 == 0) && (static_cast<int>(globalZ) % 17 == 0))
			{
				for (int y = surfaceHeight + 1; y <= surfaceHeight + 3 && y < CHUNK_HEIGHT; y++)
				{
					blocks[x][y][z] = BlockId::Cobblestone;
				}
			}

			if (x > 1 && x < CHUNK_WIDTH - 2 && z > 1 && z < CHUNK_DEPTH - 2 &&
				(static_cast<int>(globalX) % 29 == 4) && (static_cast<int>(globalZ) % 31 == 6))
			{
				for (int y = surfaceHeight + 1; y <= surfaceHeight + 4 && y < CHUNK_HEIGHT; y++)
				{
					blocks[x][y][z] = BlockId::OakLog;
				}

				const int leavesY = surfaceHeight + 5;
				if (leavesY < CHUNK_HEIGHT)
				{
					for (int lx = x - 2; lx <= x + 2; lx++)
					{
						for (int lz = z - 2; lz <= z + 2; lz++)
						{
							if (ContainsLocalBlock(lx, leavesY, lz) && std::abs(lx - x) + std::abs(lz - z) <= 3)
							{
								blocks[lx][leavesY][lz] = BlockId::OakLeaves;
							}
						}
					}
				}
			}
		}
	}
	MarkDirty();
}

void Chunk::BuildMesh(const BlockRegistry& blockRegistry)
{
	if (_displayListID != 0)
	{
		glDeleteLists(_displayListID, 1);
	}

	_displayListID = glGenLists(1);
	glNewList(_displayListID, GL_COMPILE);

	bool isBatchOpen = false;
	GLuint currentTexture = 0;

	auto useTexture = [&](GLuint texture)
	{
		if (!isBatchOpen)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
			currentTexture = texture;
			isBatchOpen = true;
		}
		else if (texture != currentTexture)
		{
			glEnd();
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
			currentTexture = texture;
		}
	};

	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				const BlockId blockId = blocks[x][y][z];
				if (blockRegistry.IsAir(blockId)) 
					continue;

				const float dx = static_cast<float>(x + (_chunkX * CHUNK_WIDTH));
				const float dy = static_cast<float>(y);
				const float dz = static_cast<float>(z + (_chunkZ * CHUNK_DEPTH));

				if (!blockRegistry.IsSolid(GetBlock(x, y + 1, z))) 
				{ 
					glColor3f(blockId == BlockId::Grass ? 0.404f : 1.0f, blockId == BlockId::Grass ? 0.655f : 1.0f, blockId == BlockId::Grass ? 0.239f : 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Top));
					EmitTopFace(dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlock(x, y - 1, z))) 
				{ 
					glColor3f(1.0f, 1.0f, 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Bottom));
					EmitBottomFace(dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlock(x, y, z + 1))) 
				{ 
					glColor3f(1.0f, 1.0f, 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Front));
					EmitFrontFace(dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlock(x, y, z - 1))) 
				{ 
					glColor3f(1.0f, 1.0f, 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Back));
					EmitBackFace(dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlock(x + 1, y, z))) 
				{ 
					glColor3f(1.0f, 1.0f, 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Right));
					EmitRightFace(dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlock(x - 1, y, z))) 
				{ 
					glColor3f(1.0f, 1.0f, 1.0f);
					useTexture(blockRegistry.TextureFor(blockId, BlockFace::Left));
					EmitLeftFace(dx, dy, dz);
				}
			}
		}
	}

	if (isBatchOpen)
	{
		glEnd();
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEndList();
	_isMeshBuilt = true;
}

void Chunk::Draw(const BlockRegistry& blockRegistry)
{
	
	if (!_isMeshBuilt)
	{
		BuildMesh(blockRegistry);
	}

	glCallList(_displayListID);
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

bool Chunk::IsBlockObscured(int x, int y, int z, const BlockRegistry& blockRegistry) const
{
	if (x == 0 || x == CHUNK_WIDTH - 1 ||
		y == 0 || y == CHUNK_HEIGHT - 1 ||
		z == 0 || z == CHUNK_DEPTH - 1)
	{
		return false;
	}

	return blockRegistry.IsSolid(blocks[x + 1][y][z]) &&
		blockRegistry.IsSolid(blocks[x - 1][y][z]) &&
		blockRegistry.IsSolid(blocks[x][y + 1][z]) &&
		blockRegistry.IsSolid(blocks[x][y - 1][z]) &&
		blockRegistry.IsSolid(blocks[x][y][z + 1]) &&
		blockRegistry.IsSolid(blocks[x][y][z - 1]);
}
