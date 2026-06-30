		[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const WorldEdit& edit)
		{
			ve::engine::WorldBlockEdit result{};
			result.kind = edit.kind == WorldEdit::Kind::FillBox
				? ve::engine::WorldBlockEdit::Kind::FillBox
				: ve::engine::WorldBlockEdit::Kind::SetBlock;
			result.min_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.x : edit.position.x;
			result.min_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.y : edit.position.y;
			result.min_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.minimum.z : edit.position.z;
			result.max_x = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.x : edit.position.x;
			result.max_y = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.y : edit.position.y;
			result.max_z = edit.kind == WorldEdit::Kind::FillBox ? edit.box.maximum.z : edit.position.z;
			result.block = ToInternalBlock(edit.block);
			return result;
		}

		[[nodiscard]] ve::world::TerrainGeneratorKind ToInternalTerrainGenerator(TerrainGenerator generator) noexcept
		{
			switch (generator)
			{
			case TerrainGenerator::Flat:
				return ve::world::TerrainGeneratorKind::Flat;
			case TerrainGenerator::Procedural:
			default:
				return ve::world::TerrainGeneratorKind::Procedural;
			}
		}

		[[nodiscard]] ve::world::TerrainSpawnBiome ToInternalTerrainBiome(TerrainBiome biome) noexcept
		{
			switch (biome)
			{
			case TerrainBiome::Desert:
				return ve::world::TerrainSpawnBiome::Desert;
			case TerrainBiome::Alpine:
				return ve::world::TerrainSpawnBiome::Alpine;
			case TerrainBiome::Temperate:
			default:
				return ve::world::TerrainSpawnBiome::Temperate;
			}
		}

		[[nodiscard]] ve::world::TerrainBlockPalette ToInternalTerrainPalette(TerrainPalette palette) noexcept
		{
			return ve::world::TerrainBlockPalette{
				ToInternalBlock(palette.bedrock),
				ToInternalBlock(palette.deep_stone),
				ToInternalBlock(palette.stone),
				ToInternalBlock(palette.surface),
				ToInternalBlock(palette.subsurface),
				ToInternalBlock(palette.high_elevation_surface),
				ToInternalBlock(palette.dry_surface_patch),
				ToInternalBlock(palette.gravel_patch)
			};
		}

		[[nodiscard]] ve::world::TerrainGenerationSettings ToInternalTerrainGeneration(const WorldConfig& world) noexcept
		{
			return ve::world::TerrainGenerationSettings{
				world.terrain_seed,
				world.base_surface_height,
				ToInternalTerrainBiome(world.terrain_biome),
				ToInternalTerrainGenerator(world.terrain_generator),
				ToInternalTerrainPalette(world.terrain_palette)
			};
		}

		[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept
		{
			switch (api)
			{
			case GraphicsApi::OpenGLCompatibility:
				return ve::rendering::GraphicsApi::OpenGLCompatibility;
			case GraphicsApi::DirectX12:
				return ve::rendering::GraphicsApi::DirectX12;
			case GraphicsApi::Vulkan:
			default:
				return ve::rendering::GraphicsApi::Vulkan;
			}
		}

		[[nodiscard]] ve::rendering::VulkanMinecraftDemoPreset ToInternalDemoPreset(DemoScene scene) noexcept
		{
			switch (scene)
			{
			case DemoScene::AquaModel:
				return ve::rendering::VulkanMinecraftDemoPreset::AquaModel;
			case DemoScene::SponzaAtrium:
				return ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium;
			case DemoScene::HyperrealDesert:
			default:
				return ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
			}
		}
