		"",
		true,
		{},
		{},
		{}
	};
	const std::array definitions{ authored_bricks, invalid_count, invalid_name };

	CHECK(registry.RegisterDataDefinitions(
		definitions,
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly) == 1U);
}
