#pragma once

namespace ve::assets
{
	enum class ModelCoordinateSystem
	{
		Source,
		RightHandedYUp
	};

	enum class ModelMaterialImportPolicy
	{
		PbrPreferred,
		DiffuseOnly
	};

	struct ModelImportOptions
	{
		bool triangulate = true;
		bool generate_normals = true;
		bool flip_uvs = false;
		bool optimize_meshes = true;
		float unit_scale = 1.0f;
		ModelCoordinateSystem coordinate_system = ModelCoordinateSystem::Source;
		ModelMaterialImportPolicy material_policy = ModelMaterialImportPolicy::PbrPreferred;
	};
}
