		void BuildImportedModelDemo(ve::world::World& world,
			const DemoBounds& bounds,
			std::string_view keyword,
			int base_y,
			int stage_radius,
			int target_extent,
			std::size_t voxel_budget,
			void (*fallback)(ve::world::World&, const DemoBounds&, int))
		{
			(void)bounds;
			ResetDemoWorld(world);
			const DemoBounds reset_bounds = BoundsFor(world);
			BuildModelStage(world, reset_bounds, base_y, stage_radius);
			const std::optional<std::filesystem::path> model_path = FindModelAsset(keyword);
			if (!model_path)
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World, "Model demo asset not found under assets/models for keyword: " + std::string{ keyword });
				fallback(world, reset_bounds, base_y);
				return;
			}

			ve::assets::ModelAssetLibrary library;
			std::optional<ve::assets::ImportedModel> model = library.ImportModel(*model_path);
			if (!model || !VoxelizeImportedModel(world, reset_bounds, *model, base_y, target_extent, voxel_budget))
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World, "Failed to voxelize model demo asset: " + model_path->string());
				fallback(world, reset_bounds, base_y);
				return;
			}
			VE_LOG_CATEGORY_INFO(ve::log::category::World, "Voxelized model demo asset: " + model_path->string());
		}

		void BuildAquaModelDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, "aqua", base_y, 62, 128, 700'000u, BuildAquaFallback);
		}

		void BuildSponzaAtriumDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, "sponza", base_y, 96, 170, 850'000u, BuildSponzaFallback);
		}
