#pragma once

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
