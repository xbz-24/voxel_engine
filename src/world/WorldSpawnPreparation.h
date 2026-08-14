#pragma once

#include "ChunkTypes.h"
#include "RenderMesh.h"
#include "WorldEvent.h"

#include <memory>
#include <vector>

namespace ve::rendering
{
	class RenderBackend;
}

namespace ve::world::detail
{
	struct WorldSpawnPreparation
	{
		std::vector<std::unique_ptr<ve::rendering::RenderMesh>> render_meshes;
		std::vector<WorldEvent> generated_events;
	};

	[[nodiscard]] WorldSpawnPreparation PrepareWorldSpawn(
		int world_size_chunks,
		ChunkGenerationMode generation_mode,
		const ve::rendering::RenderBackend* render_backend);
}
