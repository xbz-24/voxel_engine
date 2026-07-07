		void BuildImportedModelDemo(ve::world::World& world,
			const DemoBounds& bounds,
			const ve::rendering::VulkanMinecraftDemoProfile& profile,
			int base_y,
			void (*fallback)(ve::world::World&, const DemoBounds&, int))
		{
			(void)bounds;
			ResetDemoWorld(world);
			const DemoBounds reset_bounds = BoundsFor(world);
			BuildModelStage(world, reset_bounds, base_y, profile.model_stage_radius);
			const ModelAssetPreflight asset = PreflightModelAsset(profile);
			if (!asset)
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World,
					asset.failure_message + "; rendering authored fallback for " + std::string{ profile.display_name });
				fallback(world, reset_bounds, base_y);
				return;
			}

			ve::assets::ModelAssetLibrary library;
			std::optional<ve::assets::ImportedModel> model = library.ImportModel(*asset.path);
			if (!model)
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World,
					"Failed to import model demo asset for " + std::string{ profile.display_name } + ": " + asset.path->string());
				fallback(world, reset_bounds, base_y);
				return;
			}
			if (!VoxelizeImportedModel(world,
				reset_bounds,
				*model,
				base_y,
				profile.model_target_extent,
				profile.model_voxel_budget))
			{
				VE_LOG_CATEGORY_WARNING(ve::log::category::World,
					"Failed to voxelize model demo asset for " + std::string{ profile.display_name } + ": " + asset.path->string());
				fallback(world, reset_bounds, base_y);
				return;
			}
			VE_LOG_CATEGORY_INFO(ve::log::category::World,
				"Voxelized " + std::string{ profile.display_name } + " from " + asset.path->string());
		}

		void BuildAquaModelDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const ve::rendering::VulkanMinecraftDemoProfile profile =
				ve::rendering::VulkanMinecraftDemoProfileFor(ve::rendering::VulkanMinecraftDemoPreset::AquaModel);
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, profile, base_y, BuildAquaFallback);
		}

		void BuildSponzaAtriumDemo(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const ve::rendering::VulkanMinecraftDemoProfile profile =
				ve::rendering::VulkanMinecraftDemoProfileFor(ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium);
			const int base_y = std::max(8, config.ground_y);
			BuildImportedModelDemo(world, bounds, profile, base_y, BuildSponzaFallback);
		}
