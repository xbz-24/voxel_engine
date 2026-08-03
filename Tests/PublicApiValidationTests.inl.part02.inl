{
	voxel::Material invalid = voxel::Material::Named("invalid");
	invalid.base_color = { -0.1f, 0.5f, 1.2f, 1.0f };
	invalid.metallic = 2.0f;
	invalid.roughness = -0.1f;
	invalid.emission = -1.0f;

	voxel::MaterialLibrary materials{};
	materials.Add(invalid)
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material::Named("duplicate"))
		.Add(voxel::Material{});

	const std::vector<std::string> issues = materials.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.r must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.base_color.b must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.metallic must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.roughness must be between 0 and 1") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material 'invalid'.emission must be zero or greater") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name is duplicated: duplicate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"material name must not be empty") != issues.end());
}

TEST_CASE("public scene graph validation reports invalid hierarchy")
{
	voxel::SceneGraph graph{};
	graph.entities.push_back(voxel::Entity::Named("missing id"));
	(void)graph.AddEntity(voxel::Entity::Named("root").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("duplicate").WithId(voxel::EntityId{ 7U }));
	(void)graph.AddEntity(voxel::Entity::Named("self parent")
		.WithId(voxel::EntityId{ 8U })
		.ChildOf(voxel::EntityId{ 8U }));
	(void)graph.AddEntity(voxel::Entity::Named("missing parent")
		.WithId(voxel::EntityId{ 9U })
		.ChildOf(voxel::EntityId{ 100U }));

	const std::vector<std::string> issues = graph.Validate();

	CHECK(graph.FindEntity(voxel::InvalidEntityId) == nullptr);
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing id' must have a stable id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'duplicate' has a duplicated id") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'self parent' cannot be parented to itself") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'missing parent' references a missing parent id") != issues.end());
}

TEST_CASE("public engine config validation reports missing asset and material references")
{
	voxel::AssetCatalog assets{};
	assets.Texture("albedo", "albedo.png");

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("painted")
		.Texture("albedo")
		.NormalTexture("missing-normal"));

	voxel::SceneGraph scene_graph{};
	(void)scene_graph.AddEntity(voxel::Entity::Named("crate")
		.Model("missing-crate")
		.Material("missing-material"));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(assets)
		.WithMaterials(materials)
		.WithSceneGraph(scene_graph);

	const std::vector<std::string> issues = config.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'painted' references missing texture asset: missing-normal") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing model asset: missing-crate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing material: missing-material") != issues.end());
}

TEST_CASE("public engine config validation rejects invalid voxel render styles")
