		std::uint32_t source_camera_count = 0;
		std::uint32_t source_light_count = 0;
		std::uint32_t source_animation_count = 0;
	};

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
