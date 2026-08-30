#pragma once

#include "EngineStartupResult.h"
#include "GraphicsGeometryTypes.h"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace ve::assets { struct ImportedModel; }

namespace ve::engine
{
	struct StaticModelSceneMeshData
	{
		std::vector<ve::rendering::Vertex3D> vertices;
		std::vector<std::uint32_t> indices;
	};

	[[nodiscard]] EngineStartupResult ConvertStaticModelSceneMesh(
		const ve::assets::ImportedModel& model,
		StaticModelSceneMeshData& mesh_data);

	[[nodiscard]] EngineStartupResult ImportStaticModelScene(
		const std::filesystem::path& model_path,
		StaticModelSceneMeshData& mesh_data);
}
