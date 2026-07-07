TEST_CASE("model asset library caches imports by path and options")
{
	CountingModelImporter::import_count = 0;
	ve::assets::ModelAssetLibrary library;
	library.RegisterImporter(std::make_unique<CountingModelImporter>());

	const std::optional<ve::assets::ImportedModel> first = library.ImportModel("asset.counting");
	const std::optional<ve::assets::ImportedModel> second = library.ImportModel("asset.counting");
	ve::assets::ModelImportOptions different_options{};
	different_options.flip_uvs = true;
	const std::optional<ve::assets::ImportedModel> third = library.ImportModel("asset.counting", different_options);
	ve::assets::ModelImportOptions scaled_options{};
	scaled_options.unit_scale = 2.0f;
	const std::optional<ve::assets::ImportedModel> fourth = library.ImportModel("asset.counting", scaled_options);

	REQUIRE(first.has_value());
	REQUIRE(second.has_value());
	REQUIRE(third.has_value());
	REQUIRE(fourth.has_value());
	CHECK(CountingModelImporter::import_count == 3);
}
