#include "Chunk.h"
#include <algorithm>
#include <cmath>

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

			// 1. Continental Wave (Massive, slow-rolling changes in elevation)
			float continent = std::sin(globalX * 0.01f) * std::cos(globalZ * 0.01f) * 30.0f;

			// 2. Mountain Wave (Jagged, mid-size peaks)
			float mountains = std::sin(globalX * 0.05f + globalZ * 0.02f) * 15.0f;

			// 3. Detail Wave (Tiny bumps and hills so it isn't perfectly smooth)
			float hills = std::sin(globalX * 0.15f) * std::cos(globalZ * 0.15f) * 4.0f;

			// Add them all together! Base ground level is now 40 blocks high.
			int surfaceHeight = 40 + static_cast<int>(continent + mountains + hills);

			// Safely clamp the height just in case the math spikes
			if (surfaceHeight < 1) surfaceHeight = 1;
			if (surfaceHeight >= CHUNK_HEIGHT - 1) surfaceHeight = CHUNK_HEIGHT - 2;

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (y < surfaceHeight) blocks[x][y][z] = 2; // Dirt
				else if (y == surfaceHeight) blocks[x][y][z] = 1; // Grass
				else blocks[x][y][z] = 0; // Air
			}
		}
	}
}

void Chunk::BuildMesh(Cube& cubeManager)
{
	_displayListID = glGenLists(1);
	glNewList(_displayListID, GL_COMPILE);

	float s = 0.5f;

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetTop());
	glColor3f(0.404f, 0.655f, 0.239f); 
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) 
					continue;
				if (GetBlock(x, y + 1, z) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - s, dy + s, dz - s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - s, dy + s, dz + s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + s, dy + s, dz + s);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + s, dy + s, dz - s);
				}
			}
		}
	}
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f); 

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetBottom());
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) continue;
				if (GetBlock(x, y - 1, z) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - s, dy - s, dz - s);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + s, dy - s, dz - s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + s, dy - s, dz + s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - s, dy - s, dz + s);
				}
			}
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetFront());
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) 
					continue;
				if (GetBlock(x, y, z + 1) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - s, dy - s, dz + s);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + s, dy - s, dz + s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + s, dy + s, dz + s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - s, dy + s, dz + s);
				}
			}
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetBack());
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) 
					continue;
				if (GetBlock(x, y, z - 1) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx - s, dy - s, dz - s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx - s, dy + s, dz - s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx + s, dy + s, dz - s);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx + s, dy - s, dz - s);
				}
			}
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetRight());
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) 
					continue;
				if (GetBlock(x + 1, y, z) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx + s, dy - s, dz - s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx + s, dy + s, dz - s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx + s, dy + s, dz + s);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx + s, dy - s, dz + s);
				}
			}
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeManager.GetLeft());
	glBegin(GL_QUADS);
	for (int x = 0; x < CHUNK_WIDTH; x++) 
	{
		for (int z = 0; z < CHUNK_DEPTH; z++) 
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++) 
			{
				if (blocks[x][y][z] == 0) 
					continue;
				if (GetBlock(x - 1, y, z) == 0) 
				{ 
					float dx = x + (_chunkX * CHUNK_WIDTH); float dy = y; float dz = z + (_chunkZ * CHUNK_DEPTH);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(dx - s, dy - s, dz - s);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(dx - s, dy - s, dz + s);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(dx - s, dy + s, dz + s);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(dx - s, dy + s, dz - s);
				}
			}
		}
	}
	glEnd();

	glEndList();
	_isMeshBuilt = true;
}

void Chunk::Draw(Cube& cubeManager)
{
	
	if (!_isMeshBuilt)
	{
		BuildMesh(cubeManager);
	}

	glCallList(_displayListID);
}

int Chunk::GetBlock(int x, int y, int z)
{
	if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
	{
		return 0;
	}
	return blocks[x][y][z];
}

bool Chunk::IsBlockObscured(int x, int y, int z)
{
	
	if (x == 0 || x == CHUNK_WIDTH - 1 ||
		y == 0 || y == CHUNK_HEIGHT - 1 ||
		z == 0 || z == CHUNK_DEPTH - 1)
	{
		return false;
	}

	
	if (blocks[x + 1][y][z] != 0 && blocks[x - 1][y][z] != 0 &&
		blocks[x][y + 1][z] != 0 && blocks[x][y - 1][z] != 0 &&
		blocks[x][y][z + 1] != 0 && blocks[x][y][z - 1] != 0)
	{
		return true; 
	}

	return false; 
}
