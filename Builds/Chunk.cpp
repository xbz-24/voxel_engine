#include "Chunk.h"
#include <algorithm>
#include <cmath>
#include <vector>

using ve::blocks::BlockFace;
using ve::blocks::BlockId;
using ve::blocks::BlockRegistry;

namespace
{
	constexpr float BlockHalfSize = 0.5f;

	enum class MeshFaceDirection
	{
		Top,
		Bottom,
		Front,
		Back,
		Right,
		Left
	};

	struct MeshFace
	{
		GLuint texture;
		MeshFaceDirection direction;
		float x;
		float y;
		float z;
		float r;
		float g;
		float b;
	};

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

	/**
	 * Emits one collected face into the active immediate-mode batch.
	 *
	 * @param face Mesh face to emit.
	 */
	void EmitCollectedFace(const MeshFace& face)
	{
		glColor3f(face.r, face.g, face.b);
		switch (face.direction)
		{
		case MeshFaceDirection::Top:
			EmitTopFace(face.x, face.y, face.z);
			break;
		case MeshFaceDirection::Bottom:
			EmitBottomFace(face.x, face.y, face.z);
			break;
		case MeshFaceDirection::Front:
			EmitFrontFace(face.x, face.y, face.z);
			break;
		case MeshFaceDirection::Back:
			EmitBackFace(face.x, face.y, face.z);
			break;
		case MeshFaceDirection::Right:
			EmitRightFace(face.x, face.y, face.z);
			break;
		case MeshFaceDirection::Left:
			EmitLeftFace(face.x, face.y, face.z);
			break;
		}
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
					else if (y < 34 && orePattern % 89 == 0)
					{
						blocks[x][y][z] = BlockId::EmeraldOre;
					}
					else if (y < 38 && orePattern % 79 == 0)
					{
						blocks[x][y][z] = BlockId::LapisOre;
					}
					else if (y < 36 && orePattern % 71 == 0)
					{
						blocks[x][y][z] = BlockId::GoldOre;
					}
					else if (y < 42 && orePattern % 53 == 0)
					{
						blocks[x][y][z] = BlockId::IronOre;
					}
					else if (y < 55 && orePattern % 47 == 0)
					{
						blocks[x][y][z] = BlockId::CopperOre;
					}
					else if (orePattern % 37 == 0)
					{
						blocks[x][y][z] = BlockId::CoalOre;
					}
					else
					{
						blocks[x][y][z] = y < 18 ? BlockId::Deepslate : BlockId::Stone;
					}
				}
				else if (y < surfaceHeight)
				{
					const int patchPattern = static_cast<int>(globalX * 3.0f + globalZ * 11.0f);
					blocks[x][y][z] = patchPattern % 19 == 0 ? BlockId::Gravel : BlockId::Dirt;
				}
				else if (y == surfaceHeight)
				{
					const int surfacePattern = static_cast<int>(globalX * 5.0f + globalZ * 9.0f);
					if (surfaceHeight > 62)
					{
						blocks[x][y][z] = BlockId::Snow;
					}
					else if (surfacePattern % 29 == 0)
					{
						blocks[x][y][z] = BlockId::Sand;
					}
					else if (surfacePattern % 31 == 0)
					{
						blocks[x][y][z] = BlockId::MossBlock;
					}
					else
					{
						blocks[x][y][z] = BlockId::Grass;
					}
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

			if ((static_cast<int>(globalX) % 41 == 8) && (static_cast<int>(globalZ) % 37 == 12))
			{
				for (int y = surfaceHeight + 1; y <= surfaceHeight + 2 && y < CHUNK_HEIGHT; y++)
				{
					blocks[x][y][z] = BlockId::MossyCobblestone;
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

void Chunk::BuildMesh(const BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor)
{
	if (_displayListID != 0)
	{
		glDeleteLists(_displayListID, 1);
	}

	std::vector<MeshFace> faces;
	faces.reserve(CHUNK_WIDTH * CHUNK_DEPTH * 8);

	auto collectFace = [&](BlockId blockId, BlockFace face, MeshFaceDirection direction, float dx, float dy, float dz)
	{
		const bool isGrassTop = blockId == BlockId::Grass && face == BlockFace::Top;
		faces.push_back(MeshFace{
			blockRegistry.TextureFor(blockId, face),
			direction,
			dx,
			dy,
			dz,
			isGrassTop ? 0.404f : 1.0f,
			isGrassTop ? 0.655f : 1.0f,
			isGrassTop ? 0.239f : 1.0f
		});
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

				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x, y + 1, z, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Top, MeshFaceDirection::Top, dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x, y - 1, z, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Bottom, MeshFaceDirection::Bottom, dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x, y, z + 1, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Front, MeshFaceDirection::Front, dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x, y, z - 1, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Back, MeshFaceDirection::Back, dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x + 1, y, z, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Right, MeshFaceDirection::Right, dx, dy, dz);
				}
				if (!blockRegistry.IsSolid(GetBlockWithNeighbors(x - 1, y, z, westNeighbor, eastNeighbor, northNeighbor, southNeighbor))) 
				{ 
					collectFace(blockId, BlockFace::Left, MeshFaceDirection::Left, dx, dy, dz);
				}
			}
		}
	}

	std::sort(faces.begin(), faces.end(), [](const MeshFace& left, const MeshFace& right)
	{
		if (left.texture != right.texture)
		{
			return left.texture < right.texture;
		}
		if (left.r != right.r)
		{
			return left.r < right.r;
		}
		if (left.g != right.g)
		{
			return left.g < right.g;
		}
		return left.b < right.b;
	});

	_displayListID = glGenLists(1);
	glNewList(_displayListID, GL_COMPILE);

	GLuint currentTexture = 0;
	bool isBatchOpen = false;
	for (const MeshFace& face : faces)
	{
		if (!isBatchOpen || face.texture != currentTexture)
		{
			if (isBatchOpen)
			{
				glEnd();
			}

			glBindTexture(GL_TEXTURE_2D, face.texture);
			glBegin(GL_QUADS);
			currentTexture = face.texture;
			isBatchOpen = true;
		}

		EmitCollectedFace(face);
	}

	if (isBatchOpen)
	{
		glEnd();
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEndList();
	_isMeshBuilt = true;
}

void Chunk::Draw(const BlockRegistry& blockRegistry, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor)
{
	
	if (!_isMeshBuilt)
	{
		BuildMesh(blockRegistry, westNeighbor, eastNeighbor, northNeighbor, southNeighbor);
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

BlockId Chunk::GetBlockWithNeighbors(int x, int y, int z, const Chunk* westNeighbor, const Chunk* eastNeighbor, const Chunk* northNeighbor, const Chunk* southNeighbor) const
{
	if (y < 0 || y >= CHUNK_HEIGHT)
	{
		return BlockId::Air;
	}
	if (x < 0)
	{
		return westNeighbor ? westNeighbor->GetBlock(CHUNK_WIDTH - 1, y, z) : BlockId::Air;
	}
	if (x >= CHUNK_WIDTH)
	{
		return eastNeighbor ? eastNeighbor->GetBlock(0, y, z) : BlockId::Air;
	}
	if (z < 0)
	{
		return northNeighbor ? northNeighbor->GetBlock(x, y, CHUNK_DEPTH - 1) : BlockId::Air;
	}
	if (z >= CHUNK_DEPTH)
	{
		return southNeighbor ? southNeighbor->GetBlock(x, y, 0) : BlockId::Air;
	}

	return GetBlock(x, y, z);
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
