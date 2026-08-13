#pragma once

#include "ImportedModelDiagnostics.h"
#include "ImportedModelMaterial.h"
#include "ImportedModelMesh.h"
#include "ModelImportOptions.h"

#include "CoreTypes.h"

#include <cstdint>
#include <filesystem>

namespace ve::assets
{
	struct ImportedModel
	{
		std::filesystem::path source_path;
		bool has_root_node = false;
		ImportedNode root_node{};
		ve::core::DynamicArray<ImportedMaterial> materials;
		ve::core::DynamicArray<ImportedMesh> meshes;
		ve::core::DynamicArray<ModelImportDiagnostic> diagnostics;
		std::uint32_t source_camera_count = 0;
		std::uint32_t source_light_count = 0;
		std::uint32_t source_animation_count = 0;
	};
}
