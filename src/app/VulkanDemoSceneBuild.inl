		class CrystalJungleBuilder
		{
		public:
			CrystalJungleBuilder(ve::world::World& world, VulkanMinecraftDemoSceneConfig config)
				: world_(world),
				  config_(Sanitize(config)),
				  bounds_(BoundsFor(world))
			{
			}

			void Build()
			{
				ResetDemoWorld(world_);
				bounds_ = BoundsFor(world_);
				BuildJungleTerrain(world_, bounds_, config_);
				BuildLagoon(world_, bounds_, config_);
				BuildStreamCut(world_, bounds_, config_);
				BuildTorchCave(world_, bounds_, config_);
				BuildMossyOutcrop(world_, bounds_, config_);
				BuildGroundCover(world_, bounds_, config_);
				BuildRockFields(world_, bounds_, config_);
				BuildPebbleFields(world_, bounds_, config_);
				BuildJunglePlants(world_, bounds_, config_);
				BuildMossyRuins(world_, bounds_, config_);
				BuildWildlife(world_, bounds_, config_);
				BuildHeatGlints(world_, bounds_, config_);
				BuildSurveyMarkers(world_, bounds_, config_);
			}

		private:
			ve::world::World& world_;
			VulkanMinecraftDemoSceneConfig config_;
			DemoBounds bounds_;
		};
	}

	void VulkanDemoSceneBuilder::Build(ve::world::World& world, ve::rendering::VulkanMinecraftDemoSceneConfig config)
	{
		const VulkanMinecraftDemoSceneConfig sanitized = Sanitize(config);
		switch (sanitized.preset)
		{
		case ve::rendering::VulkanMinecraftDemoPreset::AquaModel:
			BuildAquaModelDemo(world, BoundsFor(world), sanitized);
			break;
		case ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium:
			BuildSponzaAtriumDemo(world, BoundsFor(world), sanitized);
			break;
		case ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert:
		default:
		{
			CrystalJungleBuilder builder{ world, sanitized };
			builder.Build();
			break;
		}
		}
	}
}
